# Como executar o projeto no Windows

Este projeto usa Unreal Engine **5.8** e os plugins Tempo incluídos como submódulos Git.

## 1. Clonar o projeto

```powershell
git clone --recurse-submodules <URL_DO_SEU_REPOSITORIO>
cd TempoExperimentos
```

Se o clone já foi feito sem os submódulos:

```powershell
git submodule update --init --recursive
```

## 2. Instalar e indicar o Unreal Engine

Instale o **Unreal Engine 5.8**. Em seguida, crie a variável de ambiente de sistema `UNREAL_ENGINE_PATH` apontando para a pasta raiz da instalação. Exemplo:

```text
UNREAL_ENGINE_PATH=C:\UnrealEngines\UE_5.8
```

Feche e abra novamente o VS Code depois de alterar essa variável. No PowerShell, confirme:

```powershell
$env:UNREAL_ENGINE_PATH
Test-Path "$env:UNREAL_ENGINE_PATH\Engine\Binaries\Win64\UnrealEditor.exe"
```

O segundo comando deve retornar `True`.

## 3. Preparar o Tempo (uma vez por clone)

Abra **Git Bash** na raiz do projeto e execute:

```bash
./Plugins/Tempo/Setup.sh
```

Quando o script perguntar se deve atualizar uma dependência de terceiros, responda `y`. No Windows, se o Git Bash apresentar erro de certificado do `curl`, faça a sessão usar o `curl.exe` do Windows e tente de novo:

```bash
export PATH="/c/Windows/System32:$PATH"
./Plugins/Tempo/Setup.sh
```

## 4. Compilar e abrir pelo VS Code

Instale a extensão **C/C++** da Microsoft no VS Code, abra a pasta raiz do projeto e pressione **F5**.

A configuração versionada em `.vscode/` compila `TempoSampleEditor`, abre `TempoSample.uproject` no Unreal Editor e permite depurar o Editor. Se o Editor já estiver aberto, feche-o antes do F5: a compilação pode ser bloqueada pelo Live Coding ou por arquivos em uso.

## 5. Rodar o exemplo Python/Jupyter

Depois da primeira compilação, use PowerShell na raiz:

```powershell
.\TempoEnv\Scripts\Activate.ps1
.\TempoEnv\Scripts\python.exe -m pip install --force-reinstall --no-deps .\Plugins\Tempo\TempoCore\Content\Python\API
.\TempoEnv\Scripts\python.exe -m pip install -r .\Plugins\Tempo\TempoCore\Content\Python\API\requirements.txt
.\TempoEnv\Scripts\python.exe -m pip install jupyter
jupyter lab
```

Os dois primeiros comandos garantem que o pacote `tempo_sim` e dependências como
`curio` sejam instalados no `TempoEnv` do projeto, mesmo se o prebuild tiver
usado outro Python instalado na máquina.

Abra `Content/Python/ExampleClients/TempoSimExamples.ipynb`. Primeiro inicie o Editor pelo F5 e então execute no notebook:

```python
await tempo.set_server(address="127.0.0.1", port=10001)
```

`127.0.0.1` é o endereço correto quando o notebook e o Editor rodam na mesma máquina. `0.0.0.0` é usado pelo servidor para escutar interfaces, não pelo cliente para se conectar.

## Verificação rápida da conexão

Com o mapa aberto e em Play, uma célula simples deve listar atores:

```python
atores = await tw.get_all_actors()
print(f"Conectado ao Unreal: {len(atores.actors)} atores encontrados")
```

## Problemas comuns

| Sintoma | Solução |
| --- | --- |
| `TempoCore does not support Unreal Engine 5.5` | Corrija `UNREAL_ENGINE_PATH` para uma instalação UE 5.8 e reabra o VS Code. |
| `ModuleNotFoundError: tempo` | Este projeto usa `import tempo_sim as tempo`; ative `TempoEnv`. |
| `ModuleNotFoundError: curio` | Execute `.\TempoEnv\Scripts\python.exe -m pip install curio`. |
| `UNAVAILABLE ... 10001` | Inicie o Editor, abra o mapa e entre em Play antes de executar a célula. |
| `Connection reset` ao solicitar imagens da câmera | O Editor encerrou ou o serviço gRPC foi interrompido; teste primeiro a verificação rápida acima e consulte `Saved/Logs`. |
