Relatório Técnico: Inconsistência de PATH e Binários de Sistema no Debian 12


1. Contexto do Problema

Ao utilizar o SDK da Milk-V em uma instalação limpa do Debian 12, o processo de geração de imagem falha ao tentar executar o utilitário mkdosfs. O sistema reporta que o comando não foi encontrado (sh: 1: mkdosfs: not found), mesmo após a instalação confirmada do pacote dosfstools.


2. Causa Raiz: Separação de Privilégios no PATH

No Debian, existe uma distinção rigorosa entre binários de usuário comum e binários de administração do sistema:

    /usr/bin: Contém ferramentas gerais (executáveis por qualquer usuário).

    /usr/sbin: Contém ferramentas de sistema e manutenção de disco (como mkfs, fdisk, mkdosfs).

Por padrão, o perfil de usuário comum no Debian não inclui os diretórios sbin na variável de ambiente $PATH. Como o script build.sh e o utilitário genimage são executados como usuário comum, eles não conseguem "enxergar" ferramentas localizadas em /usr/sbin.


3. Evidência Técnica

    Usuário Comum: which mkdosfs retorna vazio.

    Usuário Root (sudo su): which mkdosfs retorna /usr/sbin/mkdosfs.

    Código de Erro: O Shell retorna o erro 127, indicando que o executável não foi localizado no escopo do PATH atual.


4. Soluções Aplicadas
A: Solução Imediata (Link Simbólico)

A criação de um link simbólico de /usr/sbin para /usr/bin resolve o problema de forma definitiva para o sistema, tornando o binário visível para usuários sem privilégios de root.
Bash

sudo ln -sf /usr/sbin/mkdosfs /usr/bin/mkdosfs
sudo ln -sf /usr/sbin/mkfs.vfat /usr/bin/mkfs.vfat

B: Solução de Sessão (Ajuste de PATH)

Adicionar manualmente os diretórios de sistema ao PATH da sessão atual do terminal:
Bash

export PATH=$PATH:/usr/sbin:/sbin


C: Execução com Privilégios Elevados (sudo su) - NÃO RECOMENDADA

Esta opção consiste em elevar os privilégios de todo o ambiente de build para o usuário Root antes de iniciar o script. Embora resolva o problema do PATH (já que o Root enxerga os diretórios sbin), ela introduz riscos significativos ao sistema e ao código.
Bash

sudo su
./build.sh lunch

Por que esta opção não é recomendada?

    Poluição de Permissões: Ao compilar como Root, todos os arquivos gerados (objetos .o, binários e o Kernel) pertencerão ao Root. Se você precisar editar um arquivo depois como seu usuário comum (zeviani), você será bloqueado por "Permissão Negada".

    Risco ao Sistema Host: O script de build executa comandos de limpeza (rm -rf). Se houver um erro de digitação ou uma variável mal definida no script do SDK, rodar como Root dá permissão para o script apagar pastas vitais do seu Debian, e não apenas da pasta do projeto.

    Dependências de Ambiente: Muitas ferramentas de build (como o próprio Buildroot) possuem travas de segurança que impedem a execução como Root para evitar que o ambiente de compilação seja contaminado por configurações globais do sistema.

    Criação de Arquivos de Dispositivo: O processo de criação de imagem frequentemente usa ferramentas como o fakeroot justamente para simular permissões de root sem precisar ser root de verdade. Rodar o script inteiro como sudo quebra essa lógica de isolamento.


          Resumo das Opções para o Relatório:

Opção  	Método	      Recomendação	                          Motivo

A	  Link Simbólico	  Altamente Recomendada      	Resolve o problema do PATH de forma limpa e permanente.
B	  Export PATH	      Recomendada	                Segura e temporária (dura apenas enquanto o terminal estiver aberto).
C	  Sudo Su / Root  	Não Recomendada            	Gera problemas de permissão nos arquivos e oferece risco ao sistema host.
