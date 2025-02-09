# Funcionamento da Simulação Wokwi:
O intuito desta tarefa é testar os conhecimentos sobre o funcionamento da placa, assim como das conexões I2C e UART.
# Como funciona:
> Apertar o Botão Preto, fará com que o LED Verde mude de estado. Um aviso aparecerá no Display avisando, uma mensagem também aparecerá no Terminal.
> Apertar o Botão Verde, fará com que o LED Azul mude de estado. Um aviso aparecerá no Display avisando, uma mensagem também aparecerá no Terminal.
> Se algum caractere for digitado no Terminal, este será visualizado no Display.
>> Apenas um caractere aparecerá por vez.
>> Existe distinção entre os caracteres maiusculos e minusculos (Case Sensitive).
>> Caracteres de pontuação e especiais não estão disponíveis.
>> Caso seja um caractere numérico, o mesmo também aparecerá na Matriz de LEDs da placa.
>>> A matriz só mudará de estado quando outro caractere for digitado.
>>>> Caso seja um caractere não numérico, a Matriz será desligada.
>>>> Apertar algum botão não alterará o estado da Matriz.