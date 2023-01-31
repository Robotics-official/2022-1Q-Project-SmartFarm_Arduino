import requests
userid=2468
#장치 추가
#res = requests.post('http://127.0.0.1:5000/adddevice/1234', json={ "case":1, "port":2, "switch":False,"value":35,"value2":70})

#장치 삭제
#res = requests.get('http://127.0.0.1:5000/delete/1234/1')

#스위치 온오프
#res = requests.get('http://127.0.0.1:5000/switch/1234/1')

#값 업데이트
res = requests.post('http://34.64.235.122/update/1234/3', json={ "value":10,"value2":10})

#res = requests.post(f'http://34.64.235.122/adddevice/{userid}', 
#json={ "case":50, "port":3, "switch":True,"value":0,"value2":0})

#device에서 서버 값 읽기
# response=requests.get('http://34.64.235.122/read/1234')

#문자열을 딕셔너리로 전환
# a=(response.text[1:-1].split('}, {'))

# for idx,sense in enumerate(a):
#     if len(a)==1:
#         break
#     elif idx==0:
#         a[idx]=sense+'}'
#     elif idx==len(a)-1:
#         a[idx]='{'+sense
#     else:
#         a[idx]='{'+sense+'}'

# b=[]
# for i in a:
#     b.append(eval(i))
#값 확인
# print(b[1])
# print(type(b[1]))
# print(b[1].keys())
# print(b[1].values())
# print(b[1]['id'])