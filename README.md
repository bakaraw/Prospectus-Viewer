# Assembly project

- enrollment system blah blah

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