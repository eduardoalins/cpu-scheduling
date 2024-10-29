## Sobre o código

Código desenvolvido na cadeira de Infraestrutura de Software do 3º período do curso de ciência da computação da CESAR School.

O objetivo da atividade era passar nos testes, cumprir com os requisitos descritos no documento CPUScheduling.pdf enviado pelo professor e enviar até o deadline.

Outros requisitos/informações dadas pelo professor que não foram descritos no documento:
```
A ordem de impressão das tarefas nas listas LOST DEADLINES, COMPLETE EXECUTION e KILLED é a mesma do arquivo de entrada.
LOST vem antes de KILL, ou seja, a verificação do LOST deve vir antes do KILL.
Finish (F), depois Hold (H) e na sequência (L)
```

O código simula o funcionamento de 2 algoritmos de escalonamento de CPU, o Rate Monotonic e o Earliest Deadline First.

## Para rodar

Primeiro faça o make do algoritimo desejado: `make rate` ou `make edf`

Depois rode em ambiente linux passando o arquivo de teste: `./rate teste.txt` ou `./edf teste.txt` ou se preferir execute o teste.sh para rodar os testes para validar a implementação.
