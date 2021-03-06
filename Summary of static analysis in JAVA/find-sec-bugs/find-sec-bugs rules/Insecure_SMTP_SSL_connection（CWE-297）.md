###Insecure SMTP SSL connection
不安全的邮件SSL连接
**Bug Pattern：** INSECURE_SMTP_SSL
####描述：
进行SSL连接时，服务器身份验证被禁用。 一些启用SSL连接的电子邮件库默认情况下不验证服务器证书。 这相当于信任所有证书。 当试图连接到服务器时，这个应用程序很容易接受发给“hackedserver.com”的证书。 现在，应用程序可能会将破坏的SSL连接上的敏感用户信息泄露给被黑客入侵的服务器。

- [x] 安全相关

####漏洞代码：
```
...

Email email = new SimpleEmail();

email.setHostName("smtp.servermail.com");

email.setSmtpPort(465);

email.setAuthenticator(new DefaultAuthenticator(username, password));

email.setSSLOnConnect(true);

email.setFrom("user@gmail.com");

email.setSubject("TestMail");

email.setMsg("This is a test mail ... :-)");

email.addTo("foo@bar.com");

email.send();

...

```
####解决方案：
Please add the following check to verify the server cerfiticate:

```
email.setSSLCheckServerIdentity(true);

```
#### Check级别：AST

####相关信息：
1. [CWE-297: Improper Validation of Certificate with Host Mismatch](https://cwe.mitre.org/data/definitions/297.html)


