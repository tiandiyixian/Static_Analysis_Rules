###Unsafe Jackson deserialization configuration

不安全的Jackson反序列化配置

**Bug Pattern：**JACKSON_UNSAFE_DESERIALIZATION
####描述：
如果Jackson数据绑定库使用不正确，则不受信任的数据的反序列化可导致远程代码执行，如果类路径中存在允许触发恶意操作的类。

- [x] 安全相关

####风险代码：
```
public class Example {

    static class ABean {

        public int id;

        public Object obj;

    }



    static class AnotherBean {

        @JsonTypeInfo(use = JsonTypeInfo.Id.CLASS) // or JsonTypeInfo.Id.MINIMAL_CLASS

        public Object obj;

    }



    public void example(String json) throws JsonMappingException {

         ObjectMapper mapper = new ObjectMapper();

         mapper.enableDefaultTyping();

         mapper.readValue(json, ABean.class);

    }



    public void exampleTwo(String json) throws JsonMappingException {

         ObjectMapper mapper = new ObjectMapper();

         mapper.readValue(json, AnotherBean.class);

    }



}

```
#### Check级别：AST

####解决方案：
Explicitly define what types and subtypes you want to be available when using polymorphism through JsonTypeInfo.Id.NAME. Also, never call ObjectMapper.enableDefaultTyping (and then readValue a type that holds a Object or Serializable or Comparable or a known deserialization type).

####相关信息：
1. [Jackson Deserializer security vulnerability]( https://github.com/FasterXML/jackson-databind/issues/1599)
2. [Java Unmarshaller Security - Turning your data into code execution](https://github.com/mbechler/marshalsec)

