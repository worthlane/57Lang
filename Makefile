FRONT = makefront
BACK  = makeback
MID   = makemid

all: front back mid

front:
	make -f $(FRONT) makedirs && make -f $(FRONT)

back:
	make -f $(BACK) makedirs && make -f $(BACK)

mid:
	make -f $(MID) makedirs && make -f $(MID)

.PHONY: clean

clean:
	make -f $(FRONT) clean && make -f $(BACK) clean && make -f $(MID) clean 
