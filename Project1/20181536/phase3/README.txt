[system programming lecture]

-project 1 baseline

csapp.{c,h}
        CS:APP3e functions

myshell.{c,h}
        Simple shell example

Project 1) Phase 3 - Run Processes in Background
Phase 2에서 만든 쉘에 명령어를 백그라운드로 돌릴 수 있도록 기능을 추가한다.
명령어를 parsing하는 과정에서 백그라운드 명령어 여부를 먼저 확인한다.
백그라운드 명령어라면 명령어를 수행하고 바로 새로운 커맨드 라인을 출력하도록 한다.
그리고 나중에 백그라운드 프로세스가 죽으면 sigchld_handler를 통해서 reaping하도록 한다.
추가적으로 백그라운드로도 프로세스가 동작할 수 있음으로 job을 관리해주어야 한다.
jobs, bg, fg, kill 명령어를 builtin 명령어로 구현한다.
그리고 이를 위한 sigint_handler와 sigtstp_handler까지 구현하여 백그라운드 프로세스의 동작이 가능하도록 쉘을 작성한다.
