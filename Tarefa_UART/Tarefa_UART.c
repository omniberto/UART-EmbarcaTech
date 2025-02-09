#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "hardware/uart.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "pio_matrix.pio.h"

// Inicializando Conexões da Placa

#define BUTTON_A 5 // Botão A
#define BUTTON_B 6 // Botão B
#define LED_R 13 // LED Vermelho
#define LED_G 11 // LED Verde
#define LED_B 12 // LED Azul
#define MATRIZ 7 // Matriz de LEDs
#define I2C_SDA 14 // Conexão do display (data)
#define I2C_SCL 15 // Conexão do display (clock)

#define I2C_PORT i2c1 
#define ADDRESS 0x3C // Endereço

static volatile uint32_t last_time = 0; // Variável de tempo
ssd1306_t ssd; // Inicializa a estrutura do display

// Cabeçalhos das funções utilizadas

static void gpio_irq_handler(uint gpio, uint32_t events); // Função de interrupção
static void gpio_print_matrix(PIO pio, uint sm, uint flag); // Função para imprimir na matriz de LEDs
static void gpio_clear_matrix(PIO pio, uint sm); // Função para limpar a matriz de LEDs
static void setup(); // Configuração padrão
static void setup_ssd(); // Configuração Display

// Frames utilizados na mariz de LED
double frames[10][25] = {
    {0.000, 0.034, 0.034, 0.034, 0.000,
     0.000, 0.034, 0.000, 0.034, 0.000,
     0.000, 0.034, 0.000, 0.034, 0.000,
     0.000, 0.034, 0.000, 0.034, 0.000,
     0.000, 0.034, 0.034, 0.034, 0.000},

    {0.000, 0.000, 0.034, 0.000, 0.000,
     0.000, 0.000, 0.034, 0.000, 0.000,
     0.000, 0.000, 0.034, 0.000, 0.000,
     0.000, 0.000, 0.034, 0.000, 0.000,
     0.000, 0.000, 0.034, 0.000, 0.000},

    {0.000, 0.034, 0.034, 0.034, 0.000,
     0.000, 0.034, 0.000, 0.000, 0.000,
     0.000, 0.034, 0.034, 0.034, 0.000,
     0.000, 0.000, 0.000, 0.034, 0.000,
     0.000, 0.034, 0.034, 0.034, 0.000},

    {0.000, 0.034, 0.034, 0.034, 0.000,
     0.000, 0.000, 0.000, 0.034, 0.000,
     0.000, 0.034, 0.034, 0.034, 0.000,
     0.000, 0.000, 0.000, 0.034, 0.000,
     0.000, 0.034, 0.034, 0.034, 0.000},

    {0.000, 0.034, 0.000, 0.000, 0.000,
     0.000, 0.000, 0.000, 0.034, 0.000,
     0.000, 0.034, 0.034, 0.034, 0.000,
     0.000, 0.034, 0.000, 0.034, 0.000,
     0.000, 0.034, 0.000, 0.034, 0.000},

    {0.000, 0.034, 0.034, 0.034, 0.000,
     0.000, 0.000, 0.000, 0.034, 0.000,
     0.000, 0.034, 0.034, 0.034, 0.000,
     0.000, 0.034, 0.000, 0.000, 0.000,
     0.000, 0.034, 0.034, 0.034, 0.000},

    {0.000, 0.034, 0.034, 0.034, 0.000,
     0.000, 0.034, 0.000, 0.034, 0.000,
     0.000, 0.034, 0.034, 0.034, 0.000,
     0.000, 0.034, 0.000, 0.000, 0.000,
     0.000, 0.034, 0.034, 0.034, 0.000},

    {0.000, 0.034, 0.000, 0.000, 0.000,
     0.000, 0.000, 0.000, 0.034, 0.000,
     0.000, 0.034, 0.000, 0.000, 0.000,
     0.000, 0.000, 0.000, 0.034, 0.000,
     0.000, 0.034, 0.034, 0.034, 0.000},

    {0.000, 0.034, 0.034, 0.034, 0.000,
     0.000, 0.034, 0.000, 0.034, 0.000,
     0.000, 0.034, 0.034, 0.034, 0.000,
     0.000, 0.034, 0.000, 0.034, 0.000,
     0.000, 0.034, 0.034, 0.034, 0.000},

    {0.000, 0.034, 0.000, 0.000, 0.000,
     0.000, 0.000, 0.000, 0.034, 0.000,
     0.000, 0.034, 0.034, 0.034, 0.000,
     0.000, 0.034, 0.000, 0.034, 0.000,
     0.000, 0.034, 0.034, 0.034, 0.000},
};
// Transformar as informações de cores em informação para os LEDs
uint32_t matriz_rgb(double r, double g, double b) {
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

int main() {
    // Variáveis utilizadas para a matriz de LED
    
    bool set_clock;
    set_clock = set_sys_clock_khz(128000, false);

    PIO pio = pio0; 
    uint32_t valor_led;
    double r = 0.000, b = 0.000, g = 0.000;
    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, MATRIZ);

    // Configurando as conexões e o Display ssd
    setup();
    setup_ssd();
    stdio_init_all();
    // Adicionando as interrupções aos botões
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    // Laço principal
    while (true) {
        if(stdio_usb_connected()){ // Se está conectado via USB
            char c;
            if(scanf("%c", &c) == 1){ // Se há caractere lido
              ssd1306_fill(&ssd, false); // Limpa o Display
              ssd1306_draw_char(&ssd, c, 60, 28); // Desenha o caractere no Display
              printf("O caractere atual é: %c\n", c); // Mostra o caractere no Serial Monitor
            }
            if (c >= '0' && c <= '9'){ // Verifica se é um número
                gpio_print_matrix(pio0, sm, (uint)(c - '0')); // Se for, também será mostrado na matriz de LEDs
            }
            else { // Se não for
                gpio_clear_matrix(pio0, sm); // Desliga a matriz de LED
            }
        ssd1306_send_data(&ssd); // Atualiza o Display
        }
        sleep_ms(1000); // 100 ms de delay para reduzir consumo
    }
}

// Configuração e inicialização de Conexões da placa
static void setup() {
    // Configurando as conexões I2C
    i2c_init(I2C_PORT, 400 * 1000); // Inicializando
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Configurando a função do SDA
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Configurando a função do SCL
    gpio_pull_up(I2C_SDA); // Pull up the data line
    gpio_pull_up(I2C_SCL); // Pull up the clock line

    // Configurando os LEDs G e B
    gpio_init(LED_G); // LED Verde
    gpio_set_dir(LED_G, GPIO_OUT); // Configurando como Saída

    gpio_init(LED_B); // LED Azul
    gpio_set_dir(LED_B, GPIO_OUT); // Configurando como Saída

    // Configurando os Botões A e B
    gpio_init(BUTTON_A); // Botão A
    gpio_set_dir(BUTTON_A, GPIO_IN); // Configurando como Entrada
    gpio_pull_up(BUTTON_A); // Pull up para funcionamento do Botão A
    
    gpio_init(BUTTON_B); // Botão B
    gpio_set_dir(BUTTON_B, GPIO_IN); // Configurando como Entrada
    gpio_pull_up(BUTTON_B); // Pull up para funcionamento do Botão B
}
// Configuração e inicialização do Display ssd
static void setup_ssd(){
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ADDRESS, I2C_PORT); // Inicializando o Display
    ssd1306_config(&ssd); // Configurando o Display
    ssd1306_send_data(&ssd); // Enviando informação para o Display
    ssd1306_fill(&ssd, false); // Limpando o display
    ssd1306_send_data(&ssd); // Enviando informação para o Display
}
// Função de interrupção
static void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time()); // Tempo atual
    // Verifica se passou tempo suficiente desde o último evento
    if (current_time - last_time > 200000) { // 200 ms de debouncing
        last_time = current_time; // Atualiza o tempo do último evento
        ssd1306_fill(&ssd, false); // Limpa o display
        if (gpio == BUTTON_A) { // Se o botão apertado foi o Botão A
            gpio_put(LED_G, !gpio_get(LED_G)); // Muda o estado do LED Verde
            if (gpio_get(LED_G)) { // Se ele se encontra ligado
                ssd1306_draw_string(&ssd, "LED VERDE ON", 15, 28);
                printf("O LED Verde foi ligado!\n");
            }
            else { // Se ele se encontra desligado
                ssd1306_draw_string(&ssd, "LED VERDE OFF", 15, 28);
                printf("O LED Verde foi desligado!\n");
            }   
        }
        else if (gpio == BUTTON_B) { // Se o botão apertado foi o Botão B
            gpio_put(LED_B, !gpio_get(LED_B)); // Muda o estado do LED Azul
            if (gpio_get(LED_B)) { // Se ele se encontra ligado
                ssd1306_draw_string(&ssd, "LED AZUL ON", 15, 28);
                printf("O LED Azul foi ligado!\n");
            }
            else { // Se ele se encontra desligado
                ssd1306_draw_string(&ssd, "LED AZUL OFF", 15, 28);
                printf("O LED Azul foi desligado!\n");
            }
        }
        ssd1306_send_data(&ssd); // Escreve no Display
    }
}
// Função para imprimir na matriz de LEDs
static void gpio_print_matrix(PIO pio, uint sm, uint flag) {
    for (int i = 0; i < 25; i++) {
        double r = frames[flag][i]; // Intensidade de cada pixel vermelho no frame
        double g = frames[flag][i]; // Intensidade de cada pixel verde no frame
        double b = frames[flag][i]; // Intensidade de cada pixel azul no frame
        uint32_t color = matriz_rgb(r, g, b); // Informação criada para o pixel
        pio_sm_put_blocking(pio, sm, color); // Enviando para a matriz de LEDs
    }
}
// Função para desligar a matriz de LEDs
static void gpio_clear_matrix(PIO pio, uint sm){
    for (int i = 0; i < 25; i++) {
        uint32_t color = matriz_rgb(0, 0, 0);
        pio_sm_put_blocking(pio, sm, color);
    }
}