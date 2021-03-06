###Format String Manipulation
格式字符串操作

**Bug Pattern：**FORMAT_STRING_MANIPULATION
####描述：
允许用户输入控制格式参数可能使攻击者能够引发异常或者泄漏信息。

攻击者可能能够修改格式字符串参数，从而引发异常。 如果此异常未被捕获，可能会导致应用程序崩溃。 或者，如果在未使用的参数中使用敏感信息，攻击者可能会更改格式字符串以显示此信息。

- [x] 安全相关

####漏洞代码：
```
Formatter formatter = new Formatter(Locale.US);

String format = "The customer: %s %s has the balance %4$." + userInput + "f";

formatter.format(format, firstName, lastName, accountNo, balance);

```
####解决方案：
Avoid using user controlled values in the format string argument.

#### Check级别：AST

####相关信息：
1. [CWE-134: Use of Externally-Controlled Format String](http://cwe.mitre.org/data/definitions/134.html)

