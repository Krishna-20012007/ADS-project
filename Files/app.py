from flask import Flask, request, jsonify
from flask_cors import CORS
import subprocess

app = Flask(__name__)
CORS(app)

users = []
graph = {}

@app.route('/reset', methods=['POST'])
def reset():
    global users, graph
    users = []
    graph = {}
    return jsonify({"msg": "reset done"})


@app.route('/add_user', methods=['POST'])
def add_user():
    name = request.json['name']
    users.append(name)
    return jsonify({"msg": "User added"})


@app.route('/add_expense', methods=['POST'])
def add_expense():
    data = request.json
    d, c, amt = data['debtor'], data['creditor'], data['amount']

    if d not in graph:
        graph[d] = {}

    graph[d][c] = graph[d].get(c, 0) + amt
    return jsonify({"msg": "Expense added"})


@app.route('/settle', methods=['GET'])
def settle():
    input_data = "2\n"
    n = len(users)

    input_data += str(n) + "\n"
    for u in users:
        input_data += u + "\n"

    edges = []
    for d in graph:
        for c in graph[d]:
            edges.append((d, c, graph[d][c]))

    input_data += str(len(edges)) + "\n"
    for d, c, a in edges:
        input_data += f"{d} {c} {a}\n"

    result = subprocess.run(
        ["settle.exe"],
        input=input_data,
        text=True,
        capture_output=True
    )

    res = []
    for line in result.stdout.strip().split("\n"):
        if not line.strip():
            continue
        parts = line.split()
        # settle.exe outputs names directly: last token = amount,
        # second-last = "to" name, everything before = "from" name
        # This handles single-word AND multi-word names (e.g. "krishna shree")
        amount = float(parts[-1])
        to_name = parts[-2]
        from_name = " ".join(parts[:-2])
        res.append({"from": from_name, "to": to_name, "amount": amount})

    return jsonify(res)


@app.route('/search', methods=['GET'])
def search():
    prefix = request.args.get('prefix')

    input_data = "1\n"

    n = len(users)
    input_data += str(n) + "\n"

    for u in users:
        input_data += u + "\n"

    input_data += "0\n"
    input_data += prefix + "\n"

    result = subprocess.run(
        ["settle.exe"],
        input=input_data,
        text=True,
        capture_output=True
    )

    lines = result.stdout.strip().split("\n")
    filtered = [line for line in lines if line.strip()]

    return jsonify(filtered)


if __name__ == '__main__':
    app.run(debug=True)