# SmartFarm
![KakaoTalk_20220902_080730949](https://user-images.githubusercontent.com/30867895/192447758-6058c037-f596-4746-8520-d2e0adc8724a.jpg)
## client용 코드
앱과 device측에서 서버에 요청할 때 필요한 코드가 작성되어있습니다.
```python
# client용 py
#유저아이디는 송신자가 항상 동일하게 보낸다.
import requests
userid=1234
#case : 기능, port : 포트번호, switch : 전원여부, value : 센서 값
#case 숫자별 역할
# 1 : 온도/습도
# 2 : 토양습도
#50 : Fan
#51 : 펌프
#52 : LED

#APP에서 사용되는 코드

#장비 추가시 실행할 코드
res = requests.post(f'http://34.64.235.122/adddevice/{userid}', 
json={ "case":1, "port":2, "switch":False,"value":0,"value2":0})

#장비 삭제시 실행할 코드
portid=3
res = requests.get(f'http://34.64.235.122/delete/{userid}/{portid}', 

#스위치 온오프시 실행할 코드
res = requests.get(f'http://http://34.64.235.122//switch/{userid}/{portid}')

#값 업데이트시 실행할 코드
res = requests.post(f'http://http://34.64.235.122//update/{userid}/{portid}', json={ "value":10,"value2":10})

```

## DB
저장되는 데이터 형식을 지정하였습니다.
```python

#DB에 저장될 변수
class Todo(db.Model):
		#고유 번호 
    id=db.Column(db.Integer,primary_key=True)
    
		#유저 아이디
    Userid=db.Column(db.Integer)
		#기능 아이디
    Caseid=db.Column(db.Integer)
		#포트 번호
    Portid=db.Column(db.Integer)
		#스위치 ON OFF
    Switch=db.Column(db.Boolean)
		#센서값
    Value=db.Column(db.Integer)
		#센서값2
    Value2=db.Column(db.Integer)
```

## 서버
서버는 flask로 구현하였습니다.
```python
from flask import Flask, render_template,request, redirect,url_for,jsonify
from flask_sqlalchemy import SQLAlchemy
import json
import pandas as pd

app=Flask(__name__)
app.config['SQLALCHEMY_DATABASE_URI']='sqlite:///db.sqlite'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS']=False
db=SQLAlchemy(app)

#DB에 저장될 데이터 shape
class Todo(db.Model):
    id=db.Column(db.Integer,primary_key=True)
    Userid=db.Column(db.Integer)
    Caseid=db.Column(db.Integer)
    Portid=db.Column(db.Integer)
    Switch=db.Column(db.Boolean)
    Value=db.Column(db.Integer)
    Value2=db.Column(db.Integer)

#초기화면
@app.route('/')
def index():
    #show all todos
    farm_list=Todo.query.all()
    print(farm_list)
    return render_template('farmbase.html',todo_list=farm_list)

#앱에서 호출
@app.route("/adddevice/<uuid>",methods=['GET',"POST"])
def add(uuid):
    #add new item
    #json 읽어들임
    content = request.json
    #읽어온 데이터 DB에 저장
    userid=uuid
    case=content['case']
    port=content['port']
    switch=content['switch']
    value=content['value']
    value2=content['value2']
    new_todo=Todo(Userid=userid,Caseid=case,Portid=port,Switch=switch, Value=value,Value2=value2)
    db.session.add(new_todo)
    db.session.commit()
    return redirect(url_for("index"))

#앱에서 호출
@app.route("/switch/<int:uuid>/<int:port_id>")
def switch(uuid,port_id):
    #turn ON/OFF item
    #고유한 포트번호로 DB에서 찾아서 변환
    todo=Todo.query.filter_by(Portid=port_id).first()
    todo.Switch=not todo.Switch
    db.session.commit()
    return redirect(url_for("index"))

#앱에서 호출
@app.route("/delete/<int:uuid>/<int:port_id>")
def delete(uuid,port_id):
    #delete item
    #고유한 포트번호로 DB에서 찾아서 삭제
    todo=Todo.query.filter_by(Portid=port_id).first()
    db.session.delete(todo)
    db.session.commit()
    return redirect(url_for("index"))

#farm에서 호출
@app.route("/update/<int:uuid>/<int:port_id>",methods=['GET',"POST"])
def update(uuid,port_id):
    #update item
    #값을 json으로 받아 DB에서 최신화
    content = request.json
    todo=Todo.query.filter_by(Portid=port_id).first()
    todo.Value=content['value']
    todo.Value2=content['value2']
    db.session.commit()
    return redirect(url_for("index"))

#앱과 farm에서 호출
@app.route('/read/<int:uuid>')
def read(uuid):
    #show all todos
    #DB의 모든 데이터를 불러옴
    queryset = Todo.query
    df = pd.read_sql(queryset.statement, queryset.session.bind)
    #불러온 데이터를 str형식으로 출력
    return str(json.loads(df.to_json(orient='records')))

@app.route('/about')
def about():
    return "About"

if __name__=="__main__":
    #db.create_all()
    #예시
    new_todo=Todo( Userid=2468,Caseid=2, Portid=1, Switch=False,Value=56,Value2=88)
    db.session.add(new_todo)
    db.session.commit()
    app.run(debug=True)

```

## read/uuid 실행했을 때 device와 app이 얻는 json

![Untitled (1)](https://user-images.githubusercontent.com/30867895/192447948-5b654315-01f5-4ee8-bc5f-d1d433b02cf2.png)


## 구현 아이디어
1. 고유번호를 지정하기보다 포트번호로 특정데이터를 지정하는 것으로 구현하였습니다.
2. 라즈베리파이나 앱에서 데이터를 받아쓸 때 특정 값만 받는것이 아니라 전체데이터에 접근할 수 있도록 하여 필요한 데이터를 꺼내쓰도록 하였습니다. 
3. 앱이나 라즈베리파이에서 서버로 데이터를 보낼때는 장치추가의 경우를 제외하고는 데이터셋의 형식을 맞출 필요가 없습니다.
4. Userid 경우 형식상 써놓은 것이나 추후 개발로 사용자들을 구별하는 용도로 사용할 수 있습니다.(없으면 안됌)

## 데이터 흐름도
    
![흐름도](https://user-images.githubusercontent.com/30867895/192447611-390e2ffd-9679-4d26-9f46-aa6a49407fe1.png)

    

1. 앱에서 서버로 모듈 추가를 요청할 수 있습니다. 요청시 추가할 사용할 센서, 센서를 꽂을 포트번호, 사용자를 구별할 수 있는 유저 아이디를 json형식으로 함께 전달합니다. 모듈 삭제, 모듈 제어도 요청할 수 있습니다.

```python
res = requests.post(f'http://34.64.235.122/adddevice/{userid}', 
json={ "case":1, "port":2, "switch":False,"value":0,"value2":0})

```

2. 서버에서는 전달받은 json데이터를 DB에 저장합니다. 
3. 라즈베리파이에서는 서버에 저장된 DB를 읽어들여 설치된 모듈을 작동시킵니다.
4. 작동하는 모듈로부터 나오는 데이터들을 모아 서버로 전달해 서버의 DB를 최신화시킵니다.
5. 앱에서 서버의 최신화된 DB를 전달받아 사용자가 보기 쉽도록 스마트팜의 상태를 출력해줍니다.

# 서버에서는 각 Your Farm사용자의 데이터를 관리할 수 있는 관제센터기능을 제공합니다.

![Untitled](https://user-images.githubusercontent.com/30867895/192447857-106d5cd4-9284-4cf7-a5d9-02306fac60a8.png)


앱을 사용하기 어려운 상황이나 디바이스 제어에 문제가 있을 때 YourFarm관제센터에 접근하여 문제를 해결할 수 있을 것입니다.

유저 마다 고유한 의 DB를 생성하기 때문에 유저별 데이터 관리가 용이합니다.
