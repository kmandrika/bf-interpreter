Это есть простой интерпретатор языка Brainfuck (http://ru.wikipedia.org/wiki/Brainfuck) написанный на C++.

Как пользоваться:
--------------------------------------------------------------------------
Usage: [OPTION]... <source-file>

Options:
  -e, --evaluate=program    evaluate a one line program
  -i, --ignore-unknowns     ignore unknown commands within the program
  -s, --use-signed-cells    use a signed type for each cell
  -h, --help                print this message

--------------------------------------------------------------------------

$ bf -e "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>."
Hello World!
$