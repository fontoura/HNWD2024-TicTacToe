# Jogo da Velha (Tic-Tac-Toe)

> 🇺🇸 To see this file in English, click [here](./README.md)

Este repositório contém três componentes:

- Uma implementação open-source do clássico jogo da velha (Tic-Tac-Toe), jogável via linha de comando (CLI).
- Uma versão "maliciosa" do jogo que, secretamente, faz o upload de um arquivo para um servidor sem o conhecimento do usuário.
- Um servidor projetado para receber arquivos enviados pelo jogo "malicioso".

**Este software não é um malware de verdade**. Ele foi projetado para fins educacionais, especificamente para demonstrar a engenharia reversa de malware em um ambiente controlado e seguro. Embora este não seja um malware real, ele imita comportamentos maliciosos sem empregar técnicas de ofuscação ou ocultação. Essa transparência o torna ideal para aprender a identificar e analisar software potencialmente prejudicial.

Este software foi originalmente implementado para ser usado na apresentação de [ThaySolis](https://github.com/ThaySolis) no Hacking na Web Day 2024 - edição São Paulo (HNWD 2024 - São Paulo), e também foi apresentado no village da Menina de Cybersec (MSC), na Hackers to Hackers Conference de 2024 (H2HC 2024).

Para maximizar o valor educacional, recomendo fortemente que você não olhe o código-fonte. Em vez disso, construa o projeto (consulte a seção **Compilação e implantação** abaixo) e tente fazer a engenharia reversa por conta própria.

Este software foi testado em Windows e Linux.

## Compilação e implantação

A versão "maliciosa" deste software consiste em duas partes:
1. O jogo: Uma implementação modificada do jogo da velha que faz upload de arquivos para um servidor.
2. O servidor: Um programa que recebe os arquivos enviados pelo jogo malicioso.

Para preparar o ambiente, você precisa:
- Implantar o servidor em um ambiente público.
- Atualizar o código-fonte do jogo para apontar para o servidor implantado.
- Compilar o jogo.

Depois de concluído, o jogo malicioso estará pronto para engenharia reversa.

### Servidor

O código do servidor está localizado na pasta `server`. Ele aguarda conexões na porta 8000 por padrão, embora você possa modificar isso no código-fonte, caso seja necessário.

Para iniciar o servidor:
1. Implante-o em uma máquina ou ambiente em nuvem com acesso público.
2. Execute o seguinte comando:

```bash
python3 server.py
```

### Cliente

O código do jogo está localizado na pasta `game`.

#### Requisitos

Certifique-se de que as seguintes ferramentas e bibliotecas estejam instaladas e disponíveis no seu `PATH`:

- `gcc`
- `make`
- `libcurl4-openssl-dev`

#### Configurações

A pasta do jogo contém um arquivo chamado `mal_settings.h`, que define o valor de `BASE_URL`. Edite este arquivo e atualize `BASE_URL` para apontar para a URL do seu servidor, incluindo o endereço IP (e a porta, caso você a tenha alterado).

#### Compilação

Para construir o jogo, execute:

```bash
make  
```

Isso produzirá dois executáveis:
- `tictactoe`: A versão padrão do jogo, sem nenhuma funcionalidade "maliciosa".
- `tictactoe_mal`: A versão "maliciosa", que lê um arquivo e o envia ao servidor sem o conhecimento do usuário.

# Licença
Este projeto está licenciado sob a Licença MIT. Consulte o arquivo [LICENSE](./LICENSE) para mais detalhes.
