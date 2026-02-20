Relatório Técnico: Inconsistência de PATH e Geração de Imagem no Debian 12


1. Contexto do Problema

Ao utilizar o SDK da Milk-V no Debian 12, o processo de geração de imagem falha ao executar o utilitário mkdosfs (sh: 1: mkdosfs: not found), mesmo com o pacote dosfstools instalado.
2. Causa Raiz: Separação de Privilégios no PATH

O Debian 12 não inclui diretórios /sbin e /usr/sbin no PATH de usuários comuns. Como o script e o genimage rodam como usuário comum, eles não localizam ferramentas de sistema.

3. Soluções Propostas
Opção	Método             Recomendação	    Motivo
A	    Link Simbólico     Preferencial	    Tenta resolver o PATH de forma limpa criando atalhos em /usr/bin.
B	    Export PATH	       Temporária	    Ajusta o PATH apenas para a sessão atual do terminal.
C	    Sudo Su / Root	   Controversa	    Única que garantiu o sucesso no ambiente testado, apesar dos riscos.


4. Análise da Opção C (Execução como Root)

Embora a Opção A (Link Simbólico) seja a solução lógica, nos testes realizados, o sistema continuou reportando "not found" para o usuário comum. A Opção C foi a única que permitiu a conclusão do build.

Por que a Opção A pode ter falhado? (Hipóteses Técnicas)


    Nota: As causas abaixo são hipóteses baseadas no comportamento observado e não foram verificadas via análise de código de todos os sub-scripts do SDK:

        Isolamento de Ambiente: O utilitário genimage ou o uso de fakeroot dentro do SDK podem estar redefinindo a variável $PATH para um valor padrão de sistema, ignorando links simbólicos ou exportações feitas no shell do usuário.

        Ambiente Chroot/Container: Se o SDK utiliza algum nível de virtualização ou isolamento de diretórios, ele perde o acesso aos binários do host.

Riscos da Opção C:

    Poluição de Permissões: Arquivos gerados pertencem ao Root, bloqueando edição posterior pelo usuário comum.

    Segurança: Riscos de execução de comandos destrutivos (rm -rf) com privilégios totais sobre o sistema host.



5. Recuperação de Permissões (Pós-Build)

Caso a Opção C seja utilizada, é essencial devolver a posse dos arquivos gerados ao seu usuário comum para evitar erros de "Acesso Negado" no futuro.

Comando para correção (execute na raiz do SDK):
Bash

sudo chown -R $(whoami):$(whoami) .

(Este comando identifica seu usuário e grupo automaticamente e os aplica a todos os arquivos do projeto).

6. Conclusão

Para usuários que buscam apenas gerar a imagem final sem aprofundamento na arquitetura do script, a Opção C mostra-se o caminho mais rápido, desde que seguida da correção de permissões no item 5. Para desenvolvedores, recomenda-se investigar como o genimage herda as variáveis de ambiente do host.



Relatório técnico documentado por Zeviani durante o desenvolvimento para Milk-V Duo S.


