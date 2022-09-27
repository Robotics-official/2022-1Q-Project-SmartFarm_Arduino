from flask import Flask, render_template,request, redirect,url_for,jsonify
from flask_sqlalchemy import SQLAlchemy
import json
import pandas as pd

app=Flask(__name__)
app.config['SQLALCHEMY_DATABASE_URI']='sqlite:///db.sqlite'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS']=False
db=SQLAlchemy(app)

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
    #불러온 데이터를 json형식으로 출력

    return str(json.loads(df.to_json(orient='records')))



@app.route('/about')
def about():
    return "About"




if __name__=="__main__":
    db.create_all()
    #예시
    new_todo=Todo( Userid=2468,Caseid=2, Portid=1, Switch=False,Value=56,Value2=88)
    db.session.add(new_todo)
    db.session.commit()
    app.run(debug=True)


#venv\Scripts\activate 