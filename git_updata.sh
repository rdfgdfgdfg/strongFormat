git rm -r --cached .
git add *
echo 有如下更改
git status
echo 发布说明?
read commitMsg
git commit -m "$commitMsg"
git push origin main
echo 完成，按任意键退出
read