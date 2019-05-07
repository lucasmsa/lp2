- O programa foi feito em C, foram usados semaforos para o controle
do log.txt, e a parte de sockets foi feita baseado no que foi visto
na disciplina e codigos feitos em aula.

- Sao aceitos arquivos do tipo JPG, HTML e txt
 
- Para executar o programa no terminal do linux basta dar um make na pasta
em que o projeto esta inserido, caso isso nao funcione,
usar o comando: " gcc main_lucas_em_c.c -pthread -o server "

- Depois disso, dar ./server <porta>

- Apos isso, ir no browser e digitar 127.0.0.1/<porta>/<nome do arquivo>

- Para checar a resposta basta abrir o arquivo log.txt,
que eh atualizado a cada requisicao do usuario

- Uma resposta no browser tambem eh dada de acordo com o que o usuario requisita
se for um jpg a imagem eh printada no browser e caso seja um html, o conteudo 
desse html eh aparece no browser

- Os diretorios que estao no codigo sao relativos aos locais no pc em que
o codigo eh compilado, altera de pc para pc

-Para mais exemplos ver a pasta: Prints de como  executar o programa e do programa em execu�0�4�0�0o