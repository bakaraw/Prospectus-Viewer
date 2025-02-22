# Assembly project

- shows the prospectus for Bachelor of Secondary Education major in Filipino

## How to build this assembly code

- `cd` to the project folder

- run the command below in your `cmd`

```sh
build.bat
```

- if you are using `powershell` run the following:

```sh
cmd /c build.bat
```

> build.bat is a shell script that automatically runs the build commands for assembly
```sh
ml /c /coff .\main.asm
link /subsystem:console .\main.obj
.\main.exe
```

## About the project structure

- naa tay folder na `includes\` kani na folder diri naka butang ang mga helper function like text file reader og sa pag output sa subjects every year and semester

- naa pud `prospectus\` na folder. Diri nakabutang ang mga text file sa subjects every year and semester
