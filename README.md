# Projeto ZEUS
# Versão 1.0

=============================
Etapas do Projeto - Checklist
=============================

- [x] Inicialização do repositorio
- [x] Esqueleto do Banco de Dados
    - Estrutura de dados de 24 bytes para salvar informações.
    - Gravação dos dados direto no arquivo binario.
    - Testes iniciais com comandos INSERT e SELECT.
- [ ] Servidor de Rede TCP
    - Fazer o banco rodar em rede na porta usando 8080 Sockets.
- [ ] Driver de Conexão (SDK) 
    - Criar uma biblioteca para conseguir conectar no banco usando Python.
- [ ] Suporte a Multiplos Usuarios
    - Permitir que varias pessoas usem o banco ao mesmi tempo sem travar.
- [ ] Sistema de Proteção Contra Quedas de Energia (WAL)
    - Criar um log de segurança para não perder dados se o computador desligar do nada.
- [ ] Busca Super Rapida (Arvore B)
    - Organizar os indices em forma de arvore para encontrar dados em millisegundos.
- [ ] Compactação de dados
    - Comprimir os arquivos para economizar espaço no disco rigido.


======================
Tecnologias utilizadas
======================

- Linguagem C: Para deixar o sistema extremamente rapido e leve.

- Manipulçao de Arquivos Binarios: Salva os dados em arquivos .db e .idx (muito mais rapido que guardar em .txr).

- Comunicação por Sockets (winsock2.h): Permite que o programa rode como um servidor na rede (8080). 

- Compilador GCC: Utilizado para compilar e gerar o arquivo executavel.