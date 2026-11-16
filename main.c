#include <stdio.h>

int main(void)
{
    int memory[256] = {0};
    int cpu_state[8] = {0};
    int count = 0;
    int pc, command, address;
    FILE *file = fopen("program.txt", "r");

    if (file == NULL) {
        printf("Не удалось открыть program.txt\n");
        return 1;
    }

    while (count < 256 && fscanf(file, "%d", &memory[count]) == 1) {
        count++;
    }
    fclose(file);

    if (count < 2) {
        printf("Ошибка: в файле нет программы.\n");
        return 1;
    }

    // Команды и данные находятся в одной памяти, как в архитектуре фон Неймана.
    // pc указывает на команду, ir хранит ее код, acc хранит результат.
    cpu_state[0] = 6;             // Начинаем с шестой ячейки
    cpu_state[3] = memory[0];     // Адрес обработчика ошибок

    while (cpu_state[4] == 0) {
        pc = cpu_state[0];

        if (pc < 1 || pc >= count) {
            cpu_state[5] = 1;
            printf("Ошибка: счетчик команд вышел за границы программы.\n");
            break;
        }

        command = memory[pc];
        cpu_state[2] = command;

        if (command == 0) {
            printf("pc=%d: команда 0, operand=нет, acc=%d\n", pc, cpu_state[1]);
            cpu_state[4] = 1;
            break;
        }

        if (command < 1 || command > 5 || pc + 1 >= count) {
            cpu_state[5] = 2;
            printf("Ошибка: неизвестная или неполная команда, pc=%d.\n", pc);
            break;
        }

        address = memory[pc + 1];
        if (address < 0 || address >= 256 || ((command == 4 || command == 5) && address >= count)) {
            cpu_state[5] = 3;
            printf("pc=%d: команда %d, operand=%d, ОШИБКА: неверный адрес\n",
                   pc, command, address);
            break;
        }

        switch (command) {
            case 1:
                cpu_state[1] = memory[address];
                cpu_state[0] += 2;
                break;
            case 2:
                cpu_state[1] += memory[address];
                cpu_state[0] += 2;
                break;
            case 3:
                cpu_state[1] -= memory[address];
                cpu_state[0] += 2;
                break;
            case 4:
                cpu_state[0] = address;
                break;
            case 5:
                if (cpu_state[1] == 0) {
                    cpu_state[0] = address;
                } else {
                    cpu_state[0] += 2;
                }
                break;
        }

        printf("pc=%d: команда %d, operand=%d, acc=%d\n",
               pc, command, address, cpu_state[1]);
    }

    if (cpu_state[5] != 0) {
        if (cpu_state[3] >= 1 && cpu_state[3] < count) {
            cpu_state[0] = cpu_state[3];
            printf("Прерывание: переход на адрес %d\n", cpu_state[0]);
            if (memory[cpu_state[0]] == 0) {
                printf("pc=%d: команда 0, operand=нет, acc=%d\n",
                       cpu_state[0], cpu_state[1]);
            }
        }
        printf("Программа завершена с ошибкой.\n");
    } else {
        printf("Программа завершена.\n");
    }

    return 0;
}
