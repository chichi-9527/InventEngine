

## 添加新项目时更改属性

在配置管理器中移除 Win32/x86

### 输出目录： 
<code>
$(SolutionDir)\bin\$(Platform)-$(Configuration)\$(ProjectName)\
</code>

### 中间目录
<code>
$(SolutionDir)\bin-int\$(Platform)-$(Configuration)\$(ProjectName)\
</code>


### 语言标准

C/C++ 17

### VC++ 目录
包含目录
<code> 
	$(SolutionDir)\boost1_87_0\include
	$(SolutionDir)\Glfw34\include
	$(SolutionDir)\Glm1.0.1
</code>

库目录
<code>
	$(SolutionDir)\boost1_87_0\lib
	$(SolutionDir)\Glfw34\lib
</code>

### C/C++ 常规 附件包含目录
<code>
	$(SolutionDir)\InventEngine\src;
</code>

### C/C++ 命令行 其他选项
/utf-8 

### 链接器 输入 附加依赖项
<code>
	glfw3.lib;opengl32.lib;
</code>
