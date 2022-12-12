import requests
import json
headers = {
    'Address': r'tw-0.6+udp://connecting-address.invalid:8303',
    'Secret': r'81fa3955-6f83-4290-818d-31c0906b1118',
    'Challenge-Secret': r'81fa3955-6f83-4290-818d-31c0906b1118:tw0.6/ipv6',
    'Info-Serial': '0',
    'content-type': 'application/json'
}
data = json.dumps({
    "max_clients": 64,
    "max_players": 64,
    "passworded": False,
    "game_type": "DDrace",
    "name": "TeeFun-Test-Server",
    "map": {
        "name": "dm1",
        "sha256": "0b0c481d77519c32fbe85624ef16ec0fa9991aec7367ad538bd280f28d8c26cf",
        "size": 5805
    },
    "version": "0.6.4, 16.0.3",
    "clients": []
})
r = requests.post('https://master1.ddnet.tw/ddnet/15/register', headers=headers,data=data).text
print(r)
