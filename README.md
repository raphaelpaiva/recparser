MONGA - Compilador da linguagem Monga para LLVM
================================================

Trabalho da disciplina Compiladores 2, (MAB477) da Universidade Federal do Rio de Janeiro, ministrada por Fábio Mascarenhas em 2011/1.

Breve descrição
----------------

A partir de uma sintaxe definida para a linguagem monga, construímos o analizador léxico com o [Flex](http://en.wikipedia.org/wiki/Lex_programming_tool).

Então, escrevemos o analizador sintático (parser) utilizando uma abordagem top-down para construir a Árvore Sintática Abstrata ([AST](http://en.wikipedia.org/wiki/Abstract_syntax_tree)).

Com a AST em mãos, passamos para uma forma intermediária de representação, contruindo o Control-Flow Graph ([CFG](http://en.wikipedia.org/wiki/Control_flow_graph)), para então realizar sua transformação em Single Static Assignment ([SSA](http://en.wikipedia.org/wiki/Static_single_assignment_form)), requerida para a geração da linguagem-alvo, LLVM.

Depois disso podemos imprimir a CFG em SSA no formato LLVM.

A geração da AST e checagem de tipos foi escrita em C. Todas as etapas posteriores foram escritas em C++.

O Build do projeto é feito com [Make](http://en.wikipedia.org/wiki/Make_(software))

Estrutura do projeto
---------------------

* *ast/*
    * estruturas que definem a AST.
* *parser/*
    * código do analizador sintático e *type checker*, para a geração da AST.
    * as principais funcionalidades encontram-se no arquivo *driver.c*.
* *cfg/*
    * código para a geração do CFG.
    * suas principais funcionalidades encontram-se no arquivo *cfg_gen.cpp*.
    * as classes que definem o CFG encontram-se nos arquivos *cfg.h*, *TAC.h*, *operations.h* e *branch_operations.h*.
* *ssa/*
    * código para a transformação do CFG para a forma SSA.
    * suas principais funcionalidades encontram-se no arquivo *ssa.cpp*
    * o arquivo *rm_movs.cpp* contém código para a remoção de operações MOVE, pois estas não existem em LLVM.
* *test/*
    * arquivos de código-fonte monga para teste da geração do CFG.
* *test-codegen/*
    * arquivos de teste para a geração de código LLVM.
* *util/*
    * contém um script que gera LLVM-IR a partir de um código C, para comparação da geração de código do monga. Além da geração o script gera imagens do CFG e da Árvore de Dominância.
* *letscode.sh*
   * script simples que apenas abre o gedit com arquivos úteis para o desenvolvimento.
* *Makefile*
   * descritor de build do projeto.
* *mongac.cpp*
   * código que realiza o workflow citado na descrição do projeto acima. É o ponto de encontro de todas as etapas da geração da CFG.
* *monga-codegen.sh*
    * executa o *mongac* para gerar código llvm e o compila, gerando um binário executável.

Utilização
-----------
Para compilar o projeto, basta o comando:

> $make

A saída é o binário *bin/mongac*.

Para gerar o CFG, basta o comando:

> $bin/mongac *`<arquivo_fonte_monga>`*

O CFG será impresso na saída padrão, em formato LLVM.

Para gerar um binário executável, basta rodar o comando:

> $monga-codegen.sh *`<arquivo_fonte_monga>`*

Limitações
-----------
Por limitações de tempo e recursos humanos, o projeto não está completo e possui algumas limitações:

* Por enquanto a linguagem aceita apenas tipos inteiros para tipos de retorno de função, parâmetros e variáveis.
* O link com funções externas da libc (printf(), por exemplo) ainda não é possível.
   * A saída para este problema é implementar sempre uma função *int monga()* no arquivo-fonte monga. A saída dessa função é impressa pelo código contido no arquivo *runtime.c*, que é linkado com o binário gerado pelo *monga-codegen.sh* e sua main() chama a função *monga()* e imprime seu resultado na saída padrão, seguido de uma quebra de linha.

Agradecimentos
---------------
* Fábio Mascarenhas, pela oportunidade de cursar esta disciplina, conhecimento transmitido e empenho na ajuda no desenvolvimento.
* Flávio Costa, pela ajuda com as bruxarias de C/C++ e força nas horas em que os *seg faults* e *bad_allocs* reinavam.
