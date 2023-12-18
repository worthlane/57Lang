FRONT = makefront
BACK  = makeback

all: front back

front:
	make -f $(FRONT) makedirs && make -f $(FRONT)

back:
	make -f $(BACK) makedirs && make -f $(BACK)

.PHONY: clean

clean:
	make -f $(FRONT) clean && make -f $(BACK) clean
