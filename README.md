<div align="center">

# 🅿️ Parking Lot Management System

**Sistema de gestão de parques de estacionamento em C**

Projeto 1 de *Introdução aos Algoritmos e Estruturas de Dados* (IAED) — Instituto Superior Técnico, 2023/24

![C](https://img.shields.io/badge/language-C99-00599C?style=flat-square&logo=c)
![Tests](https://img.shields.io/badge/testes%20públicos-18%2F18-brightgreen?style=flat-square)

</div>

---

## 📖 Sobre o projeto

Aplicação de linha de comandos que gere até **20 parques de estacionamento**,
registando entradas e saídas de veículos, consultando o histórico de cada
matrícula e calculando a faturação de cada parque.

O programa lê comandos do *standard input*, uma instrução por linha, e escreve
as respostas no *standard output*. Toda a informação é mantida em memória
através de estruturas dinâmicas, sem limites estáticos ao número de veículos
ou de registos.

O enunciado completo está disponível em [enunciado.md](enunciado.md).

---

## ⚙️ Comandos

| Comando | Sintaxe | Ação |
|:---:|:---|:---|
| `q` | `q` | Termina o programa |
| `p` | `p` | Lista os parques por ordem de criação |
| `p` | `p <nome> <capacidade> <X> <Y> <Z>` | Cria um parque com o seu tarifário |
| `e` | `e <nome> <matrícula> <data> <hora>` | Regista a entrada de um veículo |
| `s` | `s <nome> <matrícula> <data> <hora>` | Regista a saída de um veículo |
| `v` | `v <matrícula>` | Lista todas as entradas e saídas de um veículo |
| `f` | `f <nome>` | Mostra a faturação diária de um parque |
| `f` | `f <nome> <data>` | Mostra os valores faturados num dia |
| `r` | `r <nome>` | Remove um parque e todos os seus registos |

Nomes de parques com espaços são delimitados por aspas (`"CC Colombo"`).
As datas usam o formato `DD-MM-AAAA` e as horas `HH:MM`.

### Exemplo

```console
$ ./proj1
p Saldanha 200 0.20 0.30 12.00
p "CC Colombo" 400 0.25 0.40 20.00
e Saldanha AA-00-AA 01-03-2024 08:34
Saldanha 199
s Saldanha AA-00-AA 01-03-2024 10:59
AA-00-AA 01-03-2024 08:34 01-03-2024 10:59 2.60
v AA-00-AA
Saldanha 01-03-2024 08:34 01-03-2024 10:59
f Saldanha
01-03-2024 2.60
q
```

---

## 💶 Regime de faturação

O tarifário de cada parque é definido por três valores crescentes
(*Z* > *Y* > *X*) e o tempo é cobrado em períodos de 15 minutos:

| Valor | Significado |
|:---:|:---|
| **X** | Valor de cada período de 15 minutos na 1.ª hora (4 períodos) |
| **Y** | Valor de cada período de 15 minutos após a 1.ª hora |
| **Z** | Valor máximo cobrado por cada 24 horas |

* Cada período de 24 horas completo é cobrado ao valor diário **Z**.
* O tempo restante é cobrado por períodos de 15 minutos (iniciados), nunca
  excedendo **Z**.
* No dia **29 de fevereiro** os parques estão sempre fechados: esse dia não é
  uma data válida nem é cobrado.

> **Exemplo** — tarifário `0.25 / 0.30 / 15.00`, entrada a `01-04-2024 08:00`
> e saída a `04-04-2024 10:00`: 3 períodos de 24 horas mais 2 horas, ou seja
> `3 × 15.00 + 4 × 0.25 + 4 × 0.30 = 47.20`.

---

## 📁 Estrutura do projeto

```text
.
├── main.c          Ciclo de leitura e despacho dos comandos
├── parse.c         Leitura dos argumentos: nomes com aspas, datas e horas
├── dates.c         Validação, comparação e conversão de datas e horas
├── parking.c       Criação, listagem e remoção de parques  ('p', 'r')
├── vehicle.c       Entradas, saídas e histórico de veículos ('e', 's', 'v')
├── billing.c       Cálculo dos valores cobrados e faturação ('f')
├── structures.h       Tipos de dados e constantes do sistema
├── project.h          Interface pública de cada módulo
├── enunciado.md       Enunciado do projeto
└── public-tests.zip   Casos de teste públicos e respetiva Makefile
```

---

## 🏗️ Arquitetura

O estado do sistema vive numa única estrutura `System`, passada explicitamente
a cada comando.

```text
System
 ├── parks[20] ──▶ Park ──▶ Registry ──▶ Registry ──▶ ...   (histórico do parque)
 │                   └──▶ DailyBill ──▶ DailyBill ──▶ ...   (faturação por dia)
 ├── vehicles[10007] ──▶ Vehicle ──▶ Vehicle ──▶ ...        (tabela de dispersão)
 │                          └──▶ Registry ⇄ Registry ⇄ ...  (histórico do veículo)
 └── último instante registado no sistema
```

Cada **`Registry`** representa uma permanência (entrada + saída) e é
partilhado por três listas ligadas — a do parque, a do veículo e a do dia em
que foi faturado — evitando duplicar informação.

As matrículas são procuradas numa tabela de dispersão, pelo que as entradas e
saídas são registadas em tempo constante. Como os registos são sempre
cronológicos, o histórico e a faturação diária são construídos já ordenados.

---

## 🚀 Compilação e execução

```console
$ gcc -O3 -Wall -Wextra -Werror -Wno-unused-result -o proj1 *.c
$ ./proj1 < entrada.txt > saida.txt
```

---

## 🧪 Testes

Os casos de teste públicos são executados através da Makefile incluída no
`public-tests.zip`:

```console
$ unzip public-tests.zip
$ cd public-tests
$ make
```

**Resultado: 18/18 testes públicos aceites.**

---

## 📄 Licença

O código-fonte é distribuído sob a licença [MIT](LICENSE).

O `enunciado.md` e o `public-tests.zip` são material didático do Instituto
Superior Técnico, incluídos apenas como contexto, e não são abrangidos por
essa licença.

---

## 👤 Autor

**ist1110573** (João Ferreira)  
Instituto Superior Técnico · IAED 2023/24
