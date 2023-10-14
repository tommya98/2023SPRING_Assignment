[system programming lecture]

-project 1 baseline

csapp.{c,h}
        CS:APP3e functions

myshell.{c,h}
        Simple shell example

Project 1) Phase 1 - Building and Testing Your Shell
사용자에게 입력받은 명령을 수행하는 기본적인 쉘을 만든다.

사용자에게 입력받은 명령어가 builtin 명령어인지 확인한다.
Builtin 명령어라면 쉘 프로세스에서 직접 작업을 수행한다.
사용 가능한 builtin 명령어는 exit, cd, history, !!, !# 명령어가 있다.
History 구현을 위해서 실행파일과 같은 디렉토리에 history.txt 파일을 만들어 history 내역을 관리한다.
Builtin 명령어가 아니라면 자식 프로세스를 fork한 뒤 자식 프로세스에서 exec해준다.
이때 자식 프로세스가 종료되기 전까지 부모 프로세스는 wait한 뒤 자식 프로세스가 끝나면 reaping한다.