# Quick Start - Matrix GF(2)

Быстрое руководство для начинающих разработчиков.

## За 5 минут к работающим тестам

### Шаг 1: Подготовка (1 минута)

```bash
# Перейдите в директорию проекта
cd matrix_gf2

# Создайте директорию для сборки
mkdir build
cd build
```

### Шаг 2: Конфигурация (1 минута)

```bash
# Сконфигурируйте проект
cmake ..
```

**Результат:** Вы должны увидеть примерно такое сообщение:
```
-- The CXX compiler identification is GNU 11.2.0
-- Check for working CXX compiler: /usr/bin/c++
-- Matrix GF(2) Configuration
-- ========================================
-- Build Tests: ON
-- Build Examples: ON
-- ========================================
```

### Шаг 3: Сборка (2 минуты)

```bash
# Соберите проект
cmake --build . -j4
```

**Результат:** После успешной сборки вы должны увидеть:
```
[100%] Building CXX object CMakeFiles/test_matrix_gf2.dir/tests/test_matrix_gf2.cpp.obj
[100%] Linking CXX executable test_matrix_gf2
[100%] Built target test_matrix_gf2
```

### Шаг 4: Тестирование (1 минута)

```bash
# Запустите тесты
ctest --output-on-failure
```

**Результат при успехе:**
```
Test project /path/to/build
    Start 1: matrix_gf2_tests
1/1 Test #1: matrix_gf2_tests .................. Passed    0.05 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) = 0.05 sec
```

## Полный процесс (для чистого клона)

```bash
# Клонирование
git clone https://github.com/BORZY28/matrix_gf2.git
cd matrix_gf2

# Сборка и тестирование
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
ctest --output-on-failure

# Готово! ✓
```

## Если что-то пошло не так

### Ошибка: "cmake: command not found"

```bash
# Linux (Debian/Ubuntu)
sudo apt-get install cmake

# macOS
brew install cmake

# Windows: скачайте с https://cmake.org/download/
```

### Ошибка при компиляции

```bash
# Очистите и пересоберите
cd build
rm -rf *
cmake ..
cmake --build .
```

### Тесты не запускаются

```bash
# Убедитесь, что тесты собрались
ls -la test_matrix_gf2

# Запустите вручную
./test_matrix_gf2
```

## Полезные команды

```bash
# Пересборка одного компонента
cmake --build . --target matrix_gf2

# Полная очистка
cd ..
rm -rf build
mkdir build && cd build

# Детальный вывод сборки
cmake --build . --verbose

# Запуск конкретного теста
ctest -R "test_name" --output-on-failure

# Параллельная сборка (используйте все ядра)
cmake --build . -j$(nproc)
```

## Разработка и отладка

### Для разработки используйте Debug режим

```bash
mkdir build-debug
cd build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
ctest --output-on-failure
```

### Запуск с отладчиком (Linux/macOS)

```bash
# Если установлен GDB
gdb ./test_matrix_gf2

# Или LLDB для macOS
lldb ./test_matrix_gf2
```

## Примеры

После успешной сборки вы можете запустить примеры:

```bash
# Базовый пример
./example_basic

# Пример с гауссовой элиминацией
./example_gauss

# Пример с обратной матрицей
./example_inverse

# Образовательный пример
./example_educational
```

## Структура проекта

```
matrix_gf2/
├── build/                    ← Директория сборки (создавается вами)
├── include/matrix_gf2/       ← Заголовочные файлы (.hpp, .h)
├── src/                      ← Исходные файлы (.cpp)
├── tests/                    ← Тесты
├── examples/                 ← Примеры использования
├── CMakeLists.txt           ← Главный файл сборки
├── README.md                ← Описание проекта
├── BUILD_GUIDE.md           ← Подробное руководство сборки
└── TEST_GUIDE.md            ← Подробное руководство тестирования
```

## Проверка списков

После успешного запуска тестов вы должны увидеть ✓ напротив:

- [ ] CMake сконфигурирован
- [ ] Проект собран без ошибок
- [ ] Все тесты прошли успешно
- [ ] Примеры работают
- [ ] Вы понимаете структуру проекта

## Что дальше?

1. **Изучите документацию:**
   - `BUILD_GUIDE.md` - для расширенной конфигурации сборки
   - `TEST_GUIDE.md` - для работы с тестами
   - `DOCUMENTATION.md` - для технических деталей

2. **Запустите примеры:**
   ```bash
   ./example_basic
   ./example_gauss
   ```

3. **Модифицируйте код:**
   - Отредактируйте исходные файлы в `src/`
   - Пересоберите: `cmake --build .`
   - Протестируйте: `ctest --output-on-failure`

4. **Добавьте свои тесты:**
   - Редактируйте `tests/test_matrix_gf2.cpp`
   - Пересоберите и запустите

## Типичные команды разработчика

```bash
# Вся цепочка разработки
cd matrix_gf2/build

# Редактируете код...

# Пересобрать
cmake --build .

# Протестировать
ctest --output-on-failure

# Или быстро пересобрать и протестировать
cmake --build . && ctest --output-on-failure
```

## SOS: Критические ошибки

### Полная очистка (ядерный вариант)

```bash
# Выйдите из директории build
cd ..

# Удалите все файлы сборки
rm -rf build/

# Начните заново
mkdir build
cd build
cmake ..
cmake --build .
ctest --output-on-failure
```

### Проверка зависимостей (Linux)

```bash
# Требуемые пакеты
sudo apt-get update
sudo apt-get install build-essential cmake git

# Опционально для отладки
sudo apt-get install gdb valgrind
```

## Получение помощи

1. Проверьте этот файл (QUICK_START.md)
2. Посмотрите BUILD_GUIDE.md для детальной информации
3. Посмотрите TEST_GUIDE.md для работы с тестами
4. Создайте issue на GitHub с полным выводом ошибки

## Успехи! 🎉

Если вы видите сообщение:
```
100% tests passed, 0 tests failed out of 1
```

Поздравляем! Проект полностью настроен и готов к работе.

**Дальше:**
- Изучите примеры в `examples/`
- Модифицируйте код в `src/`
- Добавьте свои тесты
- Читайте DOCUMENTATION.md для глубокого понимания
