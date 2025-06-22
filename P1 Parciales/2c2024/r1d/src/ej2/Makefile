AS = nasm
ASFLAGS = -felf64 -Fdwarf -g
LDFLAGS = -lm
CFLAGS= -Wall -Wextra -pedantic -Og -g -Wno-unused-variable -Wno-unused-parameter -no-pie -z noexecstack
IMAGES_BASE = 002 003 012 017
BASE_URL = https://raw.githubusercontent.com/macapiaggio/macapiaggio.github.io/main/_SIMD-images/

# Crea los binarios en cuestión
all: test_c test_asm img sepia

# Corre los tests usando la implementación en C
run_c: img sepia test_c
	./test_c

# Corre los tests usando la implementación en ASM
run_asm: img sepia test_asm
	./test_asm

# Corre los tests en valgrind usando la implementación en C
valgrind_c: img sepia test_c
	valgrind  --show-reachable=yes --leak-check=full --error-exitcode=1 ./test_c \
	&& echo "No se detectaron errores de memoria"

# Corre los tests en valgrind usando la implementación en ASM
valgrind_asm: img sepia test_asm
	valgrind  --show-reachable=yes --leak-check=full --error-exitcode=1 ./test_asm \
	&& echo "No se detectaron errores de memoria"

# Borra todos los archivos generados
clean:
	rm -f test_c test_asm ej2_asm.o
	rm -f img/*.jpg outputs/*.png expected/*.png

test_c: test.c ej2.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test_asm: test.c ej2_asm.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

ej2_asm.o: ej2.asm
	$(AS) $(ASFLAGS) $^ -o $@

SQUARE_IMAGES = $(addsuffix -square.jpg, $(IMAGES_BASE))
SEPIA_IMAGES = $(addsuffix -sepia.png, $(IMAGES_BASE))

img: $(addprefix ./img/, $(SQUARE_IMAGES))
sepia: $(addprefix ./expected/, $(SEPIA_IMAGES))

./img/%-square.jpg:
	curl $(BASE_URL)$(@F) --create-dirs -o $@

./expected/%-sepia.png:
	curl $(BASE_URL)sepia/$(@F) --create-dirs -o $@

download: sepia img

.PHONY: clean run_c run_asm valgrind_c valgrind_asm download

