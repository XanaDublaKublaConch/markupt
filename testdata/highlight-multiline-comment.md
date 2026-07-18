# Multi-line comment within one fenced block

```cpp
int add(int a, int b) {
    /* this comment
       spans two lines
       and should stay highlighted as a comment
       throughout, including the "code-looking" text int x = 5; below */
    return a + b;
}
```
