[system programming lecture]

-project 1 baseline

csapp.{c,h}
        CS:APP3e functions

myshell.{c,h}
        Simple shell example

Project 1) Phase 2 - Redirection and Pipe
Phase 1에서 만든 기본적인 쉘에 파이프 명령어까지 수행가능하도록 기능을 추가한다.

쉘의 기본적인 기능은 Phase 1의 쉘의 기능과 동일하다.
명령어가 입력되면 파이프 명령어인지 확인한 뒤 명령어를 수행한다.
만약 파이프 명령어라면 먼저 | 를 기준으로 명령어를 나눈다.
파이프 명령어를 수행할 때 파이프를 열어서 첫번째 명령어의 stdout이 두번째 명령어의 stdin으로 연결되도록 해준다.
그 다음 일반적인 명령어와 마찬가지로 fork를 통해 자식 프로세스를 생성한 뒤 명령어를 수행한다.
만약에 잘라낸 두번째 명령어도 파이프 명령어라면 다시한번 같은 과정을 반복해서 수행한다.