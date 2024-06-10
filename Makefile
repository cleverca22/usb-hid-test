make-descriptor: make-descriptor.c hid.h
	gcc $< -o $@ -Werror -Wall

do_report: do_report.c
	gcc $< -o $@ -Werror
