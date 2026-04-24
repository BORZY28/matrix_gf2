# Test Guide for Matrix GF(2)

Полное руководство по запуску и отладке тестов проекта matrix_gf2.

## Быстрый старт

```bash
cd build
ctest --output-on-failure
```

## Основные команды CTest

### Запуск всех тестов

```bash
# Базовый запуск
ctest

# С выводом ошибок
ctest --output-on-failure

# С подробной информацией
ctest -VV

# Очень подробно (для отладки)
ctest --output-on-failure --verbose
```

### Запуск конкретного теста

```bash
# По названию
ctest -R "test_name" --output-on-failure

# Пример: только тесты сложения матриц
ctest -R "addition" --output-on-failure
```

### Запуск нескольких тестов

```bash
# Все тесты, содержащие "matrix"
ctest -R "matrix" --output-on-failure

# Все тесты, кроме содержащих "slow"
ctest -E "slow" --output-on-failure
```

### Запуск с параллелизмом

```bash
# Запуск тестов параллельно (8 процессов)
ctest -j 8 --output-on-failure

# Максимальное количество процессов
ctest -j $(nproc) --output-on-failure
```

## Прямой запуск тестового приложения

### Linux/macOS

```bash
# Из директории build
./test_matrix_gf2

# С аргументами (если поддерживаются)
./test_matrix_gf2 -v
./test_matrix_gf2 --verbose
```

### Windows

```powershell
# Из директории build
.\Release\test_matrix_gf2.exe
.\Debug\test_matrix_gf2.exe
```

## Расширенные опции

### Увеличение timeout'а для долгих тестов

```bash
ctest --output-on-failure --timeout 300
```

### Запуск с максимальной информацией об ошибке

```bash
# Направить вывод в файл
ctest --output-on-failure -VV > test_output.log 2>&1

# Посмотреть результаты
cat test_output.log
```

### Повторный запуск только failed тестов

```bash
# Сохранить результаты в файл
ctest --output-on-failure > test_results.txt 2>&1

# Переписать и запустить снова только failed
ctest --rerun-failed --output-on-failure
```

## Структура тестов

Файл `tests/test_matrix_gf2.cpp` содержит следующие основные тестовые блоки:

### 1. Тесты элементов поля GF(2)
- Создание элементов
- Операции сложения/вычитания
- Операции умножения/деления
- Обратные элементы

### 2. Тесты матриц
- Создание матриц
- Операции сложения
- Операции умножения
- Транспонирование
- Вычисление ранга

### 3. Тесты гауссовой элиминации
- Прямой ход
- Обратный ход
- Приведение к улучшенной ступенчатой форме (RREF)

### 4. Тесты вычисления обратной матрицы
- Обратимые матрицы
- Проверка A * A^(-1) = I
- Необратимые матрицы

### 5. Тесты вычисления определителя
- Определитель квадратных матриц
- Проверка свойств определителя

## Примеры типичных тестов

### Тест сложения матриц

```cpp
// В файле test_matrix_gf2.cpp
TEST_CASE("Matrix addition") {
    Matrix A(3, 3);
    Matrix B(3, 3);
    
    // Инициализация...
    
    Matrix C = A + B;
    REQUIRE(C.rows() == 3);
    REQUIRE(C.cols() == 3);
}
```

### Тест гауссовой элиминации

```cpp
TEST_CASE("Gaussian elimination") {
    Matrix A(4, 4);
    // Инициализация матрицы...
    
    auto result = A.gaussianElimination();
    // Проверка результата...
}
```

### Тест обратной матрицы

```cpp
TEST_CASE("Matrix inverse") {
    Matrix A(3, 3);
    // Инициализация обратимой матрицы...
    
    Matrix inv = A.inverse();
    Matrix I = A * inv;
    
    // Проверка, что A * A^(-1) = I
    REQUIRE(I.isIdentity());
}
```

## Отладка тестов

### Сборка в Debug режиме

```bash
mkdir build-debug
cd build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
ctest --output-on-failure -VV
```

### Использование GDB (Linux/macOS)

```bash
# Запуск тестов под отладчиком
gdb ./test_matrix_gf2

# Внутри GDB
(gdb) run
(gdb) bt          # backtrace - трассировка стека
(gdb) print var   # вывод переменной
(gdb) break func  # breakpoint на функцию
(gdb) continue    # продолжить выполнение
```

### Использование LLDB (macOS)

```bash
lldb ./test_matrix_gf2
(lldb) run
(lldb) bt
(lldb) p var
(lldb) breakpoint set -n func
```

### Использование Visual Studio Debugger (Windows)

1. Откройте сгенерированный .sln файл
2. Установите breakpoints
3. Нажмите F5 для запуска с отладкой

## Проверка покрытия кода тестами

### Linux с GCC

```bash
# Конфигурация с поддержкой coverage
cmake .. -DCMAKE_CXX_FLAGS="--coverage" -DCMAKE_EXE_LINKER_FLAGS="--coverage"
cmake --build .
ctest --output-on-failure

# Генерирование отчета
lcov --directory . --capture --output-file coverage.info
genhtml coverage.info --output-directory coverage_html
open coverage_html/index.html
```

## Анализ результатов тестов

### Форматирование вывода

```bash
# JSON формат (для парсинга)
ctest --output-on-failure -O test_results.json

# XML формат
ctest --output-on-failure -D ExperimentalTest
```

### Интерпретация результатов

```
Test project /path/to/build
    Start 1: test_gf_element_creation
1/100 Test #1: test_gf_element_creation ........ Passed    0.01 sec
    Start 2: test_matrix_creation
2/100 Test #2: test_matrix_creation ............ Passed    0.02 sec
...

100% tests passed, 0 tests failed out of 100
```

Статусы тестов:
- **Passed**: тест успешно прошел
- **Failed**: тест не прошел
- **Timeout**: превышено время выполнения
- **Not run**: тест не был запущен

## Решение проблем при запуске тестов

### Ошибка "test_matrix_gf2: No such file"

```bash
# Убедитесь, что тесты были собраны
cmake .. -DBUILD_TESTS=ON
cmake --build .

# Проверьте наличие файла
ls -la test_matrix_gf2
```

### Тесты падают с segfault

```bash
# Запустите в отладчике для получения backtrace
gdb --args ./test_matrix_gf2
(gdb) run
(gdb) bt
```

### Тесты зависают

```bash
# Запустите с timeout
ctest --output-on-failure --timeout 30

# Прервите текущие тесты
Ctrl+C
```

### Ошибка связывания (linking error)

```bash
# Пересоберите библиотеку
cmake --build . --target clean
cmake --build .
ctest --output-on-failure
```

### Тесты требуют файлы данных

```bash
# Убедитесь, что работаете из правильной директории
cd build
# Запустите тесты
ctest --output-on-failure
```

## Непрерывное тестирование (CI/CD)

### GitHub Actions (пример)

```yaml
name: Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Create Build Environment
        run: cmake -B ${{github.workspace}}/build
      - name: Build
        run: cmake --build ${{github.workspace}}/build
      - name: Test
        working-directory: ${{github.workspace}}/build
        run: ctest --output-on-failure
```

## Статистика тестов

### Получение статистики

```bash
# Время выполнения каждого теста
ctest --output-on-failure --time-output

# Сохранение времени выполнения
ctest --output-on-failure > test_timing.txt 2>&1
grep "Test #" test_timing.txt | grep "Passed"
```

### Анализ производительности

```bash
# Запуск с измерением времени
time ctest --output-on-failure

# Запуск только самых долгих тестов
ctest --output-on-failure -T Test --time-output | tail -20
```

## Лучшие практики

1. **Запускайте тесты перед commit'ом**
   ```bash
   ctest --output-on-failure
   ```

2. **Используйте Debug режим при разработке**
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   ```

3. **Проверяйте покрытие регулярно**
   ```bash
   ctest --output-on-failure && lcov --capture --output-file coverage.info
   ```

4. **Отслеживайте производительность**
   ```bash
   ctest --output-on-failure --time-output
   ```

5. **Документируйте новые тесты**
   - Добавляйте комментарии
   - Объясняйте что тестируется
   - Указывайте ожидаемые результаты

## Требования к тестам

Все тесты должны:
- Быть независимыми друг от друга
- Завершаться за разумное время (< 1 сек)
- Не требовать внешних ресурсов
- Быть воспроизводимыми
- Иметь понятные имена и описания
- Проверять конкретное поведение
- Быть простыми и понятными

## Дополнительная информация

- [CTest официальная документация](https://cmake.org/cmake/help/latest/manual/ctest.1.html)
- [GoogleTest документация](https://google.github.io/googletest/)
- [Catch2 документация](https://github.com/catchorg/Catch2)

## Контакты и поддержка

Если у вас возникли проблемы с тестами:
1. Проверьте эту документацию
2. Посмотрите лог полного выполнения: `ctest --output-on-failure -VV`
3. Создайте issue на GitHub с полной информацией об ошибке
