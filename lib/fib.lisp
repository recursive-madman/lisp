
(set 'fib (lambda (n) (cond ((eq n 0) 0) ((eq n 1) 1) ((+ (fib (- n 1)) (fib (- n 2)))))))
(print "fib(7): " (fib 7))
