all: part1 part2 part3 part4 part5
	
part1: part1.c
	gcc -g -std=c99 -o part1 part1.c topic_queue.c synch_bounded_queue.c bounded_queue.c

part2: part2.c
	gcc -g -std=c99 -o part2 part2.c topic_queue.c synch_bounded_queue.c bounded_queue.c

part3: part3.c
	gcc -g -std=c99 -o part3 part3.c topic_queue.c synch_bounded_queue.c bounded_queue.c file_ops.c

part4: part4.c
	gcc -g -std=c99 -o part4 part4.c topic_queue.c synch_bounded_queue.c bounded_queue.c file_ops.c

part5: part5.c
	gcc -g -std=c99 -o part5 part5.c topic_queue.c synch_bounded_queue.c bounded_queue.c file_ops.c

clean:
	$(RM) part1 *.o *~
	$(RM) part2 *.o *~
	$(RM) part3 *.o *~
	$(RM) part4 *.o *~
	$(RM) part5 *.o *~