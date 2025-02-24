#include <stdio.h>
#include <math.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "neopixel.c"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"

// Pino e canal do microfone no ADC.
#define MIC_CHANNEL 2
#define MIC_PIN (26 + MIC_CHANNEL)

// Parâmetros e macros do ADC.
#define ADC_CLOCK_DIV 96.f
#define SAMPLES 200 // Número de amostras que serão feitas do ADC.
#define ADC_ADJUST(x) (x * 3.3f / (1 << 12u) - 1.65f) // Ajuste do valor do ADC para Volts.
#define ADC_MAX 3.3f
#define ADC_STEP (3.3f/5.f) // Intervalos de volume do microfone.

// Pino e número de LEDs da matriz de LEDs.
#define LED_PIN 7
#define LED_COUNT 25

// Pinos I2C para o display OLED
#define I2C_SDA 14
#define I2C_SCL 15

#define abs(x) ((x < 0) ? (-x) : (x))

// Canal e configurações do DMA
uint dma_channel;
dma_channel_config dma_cfg;

// Buffer de amostras do ADC.
uint16_t adc_buffer[SAMPLES];

void sample_mic();
float mic_power();
uint8_t get_intensity(float v);

int main() {
  stdio_init_all();

  // Delay para o usuário abrir o monitor serial...
  sleep_ms(5000);

  // Inicialização do i2c para o display OLED
  i2c_init(i2c1, ssd1306_i2c_clock * 1000);
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);

  // Inicialização do display OLED
  ssd1306_init();

  // Preparar área de renderização para o display
  struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
  };

  calculate_render_area_buffer_length(&frame_area);

  // Buffer para o display OLED
  uint8_t display_buffer[ssd1306_buffer_length];
  
  // Limpa o display
  memset(display_buffer, 0, ssd1306_buffer_length);
  render_on_display(display_buffer, &frame_area);

  // Preparação da matriz de LEDs.
  printf("Preparando NeoPixel...");
  
  npInit(LED_PIN, LED_COUNT);

  // Preparação do ADC.
  printf("Preparando ADC...\n");

  adc_gpio_init(MIC_PIN);
  adc_init();
  adc_select_input(MIC_CHANNEL);

  adc_fifo_setup(
    true, // Habilitar FIFO
    true, // Habilitar request de dados do DMA
    1, // Threshold para ativar request DMA é 1 leitura do ADC
    false, // Não usar bit de erro
    false // Não fazer downscale das amostras para 8-bits, manter 12-bits.
  );

  adc_set_clkdiv(ADC_CLOCK_DIV);

  printf("ADC Configurado!\n\n");

  printf("Preparando DMA...");

  // Tomando posse de canal do DMA.
  dma_channel = dma_claim_unused_channel(true);

  // Configurações do DMA.
  dma_cfg = dma_channel_get_default_config(dma_channel);

  channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_16); // Tamanho da transferência é 16-bits (usamos uint16_t para armazenar valores do ADC)
  channel_config_set_read_increment(&dma_cfg, false); // Desabilita incremento do ponteiro de leitura (lemos de um único registrador)
  channel_config_set_write_increment(&dma_cfg, true); // Habilita incremento do ponteiro de escrita (escrevemos em um array/buffer)
  
  channel_config_set_dreq(&dma_cfg, DREQ_ADC); // Usamos a requisição de dados do ADC

  // Amostragem de teste.
  printf("Amostragem de teste...\n");
  sample_mic();

  printf("Configuracoes completas!\n");

  printf("\n----\nIniciando loop...\n----\n");
  while (true) {

    // Realiza uma amostragem do microfone.
    sample_mic();

    // Pega a potência média da amostragem do microfone.
    float avg = mic_power();
    avg = 2.f * abs(ADC_ADJUST(avg)); // Ajusta para intervalo de 0 a 3.3V. (apenas magnitude, sem sinal)

    uint intensity = get_intensity(avg); // Calcula intensidade a ser mostrada na matriz de LEDs.

    // Limpa a matriz de LEDs.
    npClear();

    // Se detectar som (intensity > 0), exibe mensagem no display OLED
    if (intensity > 0) {
      // Limpa o display
      memset(display_buffer, 0, ssd1306_buffer_length);
      
      // Desenha a mensagem
      ssd1306_draw_string(display_buffer, 5, 10, "Som detectado!");
      
      // Atualiza o display
      render_on_display(display_buffer, &frame_area);

      // Define a intensidade do vermelho para cada nível
      uint8_t red_intensity = 250; // Intensidade máxima do vermelho

      // Acende os LEDs de acordo com a intensidade do som
      // A matriz é 5x5, então cada linha tem 5 LEDs
      // Começamos de baixo para cima
      for (int linha = 0; linha <= (intensity > 5 ? 4 : intensity - 1); linha++) {
        // Acende os 5 LEDs da linha atual
        for (int col = 0; col < 5; col++) {
          npSetLED(linha * 5 + col, red_intensity, 0, 0);
        }
        // Diminui a intensidade do vermelho para a próxima linha
        red_intensity = (red_intensity > 50) ? red_intensity - 50 : 50;
      }
    } else {
      // Limpa o display quando não há som
      memset(display_buffer, 0, ssd1306_buffer_length);
      render_on_display(display_buffer, &frame_area);
    }

    // Atualiza a matriz.
    npWrite();

    // Envia a intensidade e a média das leituras do ADC por serial.
    printf("%2d %8.4f\r", intensity, avg);

    // Delay para não sobrecarregar o sistema.
    sleep_ms(50);
  }
  return 0;
}

/**
 * Realiza as leituras do ADC e armazena os valores no buffer.
 */
void sample_mic() {
  adc_fifo_drain(); // Limpa o FIFO do ADC.
  adc_run(false); // Desliga o ADC (se estiver ligado) para configurar o DMA.

  dma_channel_configure(dma_channel, &dma_cfg,
    adc_buffer, // Escreve no buffer.
    &(adc_hw->fifo), // Lê do ADC.
    SAMPLES, // Faz SAMPLES amostras.
    true // Liga o DMA.
  );

  // Liga o ADC e espera acabar a leitura.
  adc_run(true);
  dma_channel_wait_for_finish_blocking(dma_channel);
  
  // Acabou a leitura, desliga o ADC de novo.
  adc_run(false);
}

/**
 * Calcula a potência média das leituras do ADC. (Valor RMS)
 */
float mic_power() {
  float avg = 0.f;

  for (uint i = 0; i < SAMPLES; ++i)
    avg += adc_buffer[i] * adc_buffer[i];
  
  avg /= SAMPLES;
  return sqrt(avg);
}

/**
 * Calcula a intensidade do volume registrado no microfone, de 0 a 4, usando a tensão.
 */
uint8_t get_intensity(float v)
{
  // Ajustando a sensibilidade para captar melhor o som
  if (v < 0.05f) return 0;
  else if (v < 0.1f) return 1;
  else if (v < 0.15f) return 2;
  else if (v < 0.2f) return 3;
  else if (v < 0.25f) return 4;
  else return 5; // Agora retorna até 5 para ativar todas as linhas
}
