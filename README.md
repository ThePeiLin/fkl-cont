fakeLisp的continuation扩展

### 构建
```sh
mkdir build
cd build
cmake ..
make
```

### 示例
```scheme
;;十分经典的yin-yang puzzle

(import (path to lib utils))
(import (path to cont))

(let* [(yin ((lambda (cc) (princ #\@) cc)
             (call/cc (lambda (c) c))))
       (yang ((lambda (cc) (princ #\*) cc)
              (call/cc (lambda (c) c))))]
  (if (eq (getch) #\\4) nil
    (yin yang)))
```
