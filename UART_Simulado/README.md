# Funcionamento da Simulação Wokwi:
O intuito desta tarefa é testar os conhecimentos sobre o funcionamento da placa, assim como das conexões I2C e UART.
# Como funciona:
1. Apertar o Botão Preto, fará com que o LED Verde mude de estado. Um aviso aparecerá no Display avisando, uma mensagem também aparecerá no Terminal.
2. Apertar o Botão Verde, fará com que o LED Azul mude de estado. Um aviso aparecerá no Display avisando, uma mensagem também aparecerá no Terminal.
3. Se algum caractere for digitado no Terminal, este será visualizado no Display.
    1. Apenas um caractere aparecerá por vez.
    2. Existe distinção entre os caracteres maiusculos e minusculos (Case Sensitive).
    3. Caracteres de pontuação e especiais não estão disponíveis.
    4. Caso seja um caractere numérico, o mesmo também aparecerá na Matriz de LEDs da placa.
        1. A matriz só mudará de estado quando outro caractere for digitado.
        2. Caso seja um caractere não numérico, a Matriz será desligada.
        3.  Apertar algum botão não alterará o estado da Matriz.