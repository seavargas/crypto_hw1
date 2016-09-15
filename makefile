all: encrypt.c decrypt.c
	make decrypt
	make encrypt

decrypt: decrypt.c
	gcc decrypt.c -o decrypt

encrypt: encrypt.c
	gcc encrypt.c -o encrypt

git: encrypt decrypt
	git push -u origin master
