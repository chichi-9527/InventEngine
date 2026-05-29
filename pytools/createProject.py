import sys
import os
import uuid
import shutil
from pathlib import Path

def generate_guid():
    """返回带花括号的大写 GUID 字符串，例如 {9A91B1EB-0D38-4217-AFEF-D02B08A60D36}"""
    return f"{{{str(uuid.uuid4()).upper()}}}"

def replace_placeholders(content, project_name, new_guid):
    """
    替换样板 vcxproj 中的关键内容：
    - ProjectGuid
    - RootNamespace
    - 源文件名 (TemplateProject.cpp -> main.cpp)
    """
    content = content.replace(
        '<ProjectGuid>{fc80b416-5e2c-488b-baab-7966a4f35464}</ProjectGuid>',
        f'<ProjectGuid>{new_guid}</ProjectGuid>'
    )
    content = content.replace(
        '<RootNamespace>Test</RootNamespace>',
        f'<RootNamespace>{project_name}</RootNamespace>'
    )
    # 替换所有出现的 TemplateProject.cpp 为 main.cpp
    content = content.replace('TemplateProject.cpp', 'main.cpp')
    return content

def main():
    if len(sys.argv) < 2:
        print(" 错误: 必须指定项目名称。用法: python createProject.py ProjectName")
        sys.exit(1)

    project_name = sys.argv[1].strip()
    if not project_name:
        print(" 错误: 项目名称不能为空。")
        sys.exit(1)

    # 当前工作目录（应为 bat 所在的文件夹）
    work_dir = Path.cwd()
    # pytools 目录（本脚本所在目录）
    pytools_dir = Path(__file__).resolve().parent

    # 目标项目目录：work_dir/ProjectName
    project_dir = work_dir / project_name
    src_dir = project_dir / 'src'

    # 样板文件路径
    template_vcxproj = pytools_dir / 'TemplateProject.vcxproj'
    template_filters = pytools_dir / 'TemplateProject.vcxproj.filters'
    template_user = pytools_dir / 'TemplateProject.vcxproj.user'
    template_main_cpp = pytools_dir / 'main.cpp'

    # 检查样板文件是否存在
    for tpl in [template_vcxproj, template_filters, template_user, template_main_cpp]:
        if not tpl.exists():
            print(f" 错误: 找不到样板文件 {tpl.name}，请确保 pytools 目录下存在所需文件。")
            sys.exit(1)

    # 生成新 GUID
    new_guid = generate_guid()

    # 创建目录结构
    try:
        project_dir.mkdir(parents=True, exist_ok=True)
        src_dir.mkdir(parents=True, exist_ok=True)
    except OSError as e:
        print(f" 无法创建目录: {e}")
        sys.exit(1)

    # 1. 生成 main.cpp（直接复制样板代码）
    shutil.copy(template_main_cpp, src_dir / 'main.cpp')

    # 2. 生成 .vcxproj
    raw_vcxproj = template_vcxproj.read_text(encoding='utf-8')
    vcxproj_content = replace_placeholders(raw_vcxproj, project_name, new_guid)
    (project_dir / f'{project_name}.vcxproj').write_text(vcxproj_content, encoding='utf-8')

    # 3. 生成 .vcxproj.filters
    raw_filters = template_filters.read_text(encoding='utf-8')
    filters_content = raw_filters.replace('TemplateProject.cpp', 'main.cpp')
    # filters 中还有 UniqueIdentifier，一般不需要修改，若想避免与其它项目冲突也可重新生成，但非必须。
    (project_dir / f'{project_name}.vcxproj.filters').write_text(filters_content, encoding='utf-8')

    # 4. 生成 .vcxproj.user（内容无需修改，直接复制）
    shutil.copy(template_user, project_dir / f'{project_name}.vcxproj.user')

    print(f" 项目 '{project_name}' 创建成功！")
    print(f"  位置: {project_dir}")
    print(f"  GUID: {new_guid}")

if __name__ == '__main__':
    main()