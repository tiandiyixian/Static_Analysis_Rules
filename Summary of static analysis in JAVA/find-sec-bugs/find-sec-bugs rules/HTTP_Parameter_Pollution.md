###HTTP Parameter Pollution
HTTP参数污染
**Bug Pattern：**HTTP_PARAMETER_POLLUTION
####描述：
将未经验证的用户输入连接到一个URL可以让攻击者覆盖请求参数的值。 攻击者可以覆盖现有参数值，注入新参数或利用直接范围外的变量。 HTTP参数污染（HPP）攻击由将编码查询字符串分隔符注入其他现有参数组成。 如果Web应用程序没有正确处理用户输入，恶意用户可能会损害应用程序的逻辑以执行客户端或服务器端攻击。

- [x]安全相关
####漏洞代码：
```
String lang = request.getParameter("lang");

GetMethod get = new GetMethod("http://www.host.com");

get.setQueryString("lang=" + lang + "&user_id=" + user_id);

get.execute();

```
####解决方案：
Sanitize user input before using it in HTTP parameters.

#### Check级别：AST


####相关信息：
1. [CAPEC-460: HTTP Parameter Pollution (HPP)](https://capec.mitre.org/data/definitions/460.html)




