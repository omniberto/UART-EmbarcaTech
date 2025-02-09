# Funcionamento da Placa:
O intuito desta tarefa é testar os conhecimentos sobre os componentes da placa, assim como das conexões I2C e UART.
# Como funciona:
> Apertar o Botão A, fará com que o LED Verde mude de estado. Um aviso aparecerá no Display avisando, uma mensagem também aparecerá no Serial Monitor.

> Apertar o Botão B, fará com que o LED Azul mude de estado. Um aviso aparecerá no Display avisando, uma mensagem também aparecerá no Serial Monitor.

> Se algum caractere for digitado no Serial Monitor, este será visualizado no Display.
>> Apenas um caractere aparecerá por vez.

>> Existe distinção entre os caracteres maiusculos e minusculos (Case Sensitive).

>> Caracteres de pontuação e especiais não estão disponíveis.

>> Caso seja um caractere numérico, o mesmo também aparecerá na Matriz de LEDs da placa.
>>> A matriz só mudará de estado quando outro caractere for digitado.

>>>> Caso seja um caractere não numérico, a Matriz será desligada.

>>>> Apertar algum botão não alterará o estado da Matriz.