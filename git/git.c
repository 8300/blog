//---------------------git.c---------------------------
//---------------------git the least update
git clone <url> --depth 1
//---------------------

git clone https://git.code.tencent.com/ts/note.git
.....
//---------------------update
git add .
git commit -m alter
git push
username:ts
password:54mingwang
//---------------------get repository
git init
git clone
//---------------------commit
//---------------------get message
git help
git status
git diff
git log
git show	//show the conditions change
####
设置 user.name 和 user.email
git config --global user.name "Pluto"
git config --global user.email "210214599@qq.com"
//---------------------
//---------------------
//---------------------
//---------------------
//---------------------
#error
//fatal: refusing to merge unrelated histories	//两个 根本不相干的 git 库， 一个是本地库， 一个是远端库， 然后本地要去推送到远端， 远端觉得这个本地库跟自己不相干， 所以告知无法合并
//can't update
>>:git submodule
<<:fatal: no submodule mapping found in .gitmodules for path 'ts'
git rm --cached ts

//---------------------end-----------------------------



git remote -v	//查看远程仓库地址命令
git branch -a	//查看远程分支
git branch		//查看本地分支
git pull --rebase origin master
{
git push -u origin master
git remote add origin https://gitee.com/robot_ts/robot001.git
git config --global credential.helper store//长期储存密码

//////////////////////////////////////////////////////

####### 
                
创建一个全新的仓库
git init test
cd test
touch README
git add README
git commit -m 'first commit'
git remote add origin https://gitshell.com/Pluto/test.git
git push -u origin master
                
导入现有的仓库
cd existing_git_repo
git remote add origin https://gitshell.com/Pluto/test.git
git push -u origin master
####### update from remote to local:
//方法一
git fetch origin master //从远程的origin仓库的master分支下载代码到本地的origin master
git log -p master.. origin/master//比较本地的仓库和远程参考的区别
git merge origin/master//把远程下载下来的代码合并到本地仓库，远程的和本地的合并
//方法二
git fetch origin master:temp //从远程的origin仓库的master分支下载到本地并新建一个分支temp
git diff temp//比较master分支和temp分支的不同
git merge temp//合并temp分支到master分支
git branch -d temp//删除temp
############
git init
 git clone http://210214599_qq.com_timespace@xiaolvyun.baidu.com/git/timespace/robot/robot001/.git
 git remote add origin http://210214599_qq.com_timespace@xiaolvyun.baidu.com/git/timespace/README/README/.git 
 git add fdsf
  git commit -m "test"
 git push origin master
 
 error: failed to push some refs to 'git@github.com:xxx/xxx.git'
 $ git pull --rebase origin master
$ git push -u origin master

command//
{
	git init//create .git
	git clone http:...git//downloads
	git remote add <remote_name> http:...git//add remote
	git add <filename>/\*/.//put change file to temp
	git commit -m "message"//put change message to temp(head)
	git push <remote_name> <origin_branch_name> <remote_branch_name>
}
error1//:Everything up-to-date
{
	git add fdsf
	git commit -m "test"
}

//------------error message-------------//
        Username for 'https://gitee.com': timespace
        Password for 'https://timespace@gitee.com': 
        remote: Incorrect username or password ( access token )
        fatal: Authentication failed for 'https://gitee.com/robot_ts/robot001.git/'
//------------solution-----------------//
        Username for 'https://gitee.com': 210214599@qq.com
}
git pull --rebase origin master
{
git push -u origin master
git remote add origin https://gitee.com/robot_ts/robot001.git
git config --global credential.helper store//长期储存密码
fatal: not a git repository (or any of the parent directories): .git
ts@ts-VirtualBox:~/ts/readme$ git init
Initialized empty Git repository in /home/ts/ts/readme/.git/
fatal: Not a valid object name: 'master'.
git add *.c
git commit -m 'dddd'
[master (root-commit) 49ecba9] dddd
 1 file changed, 14 insertions(+)
 create mode 100644 git.c
git branch readme
git checkout readme
Switched to branch 'readme'
git remote add readme https://gitee.com/robot_ts/robot001.git
git push readme

git push -u master test

{       //enter v587
git init
git add *.c
git commit -m 'eee'
git branch test01
git remote add master https://gitee.com/robot_ts/robot001.git
git push -u master test01
}
