make-descriptor: make-descriptor.c
	gcc $< -o $@ -Werror

do_report: do_report.c
	gcc $< -o $@ -Werror
