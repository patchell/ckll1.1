# Independent FIRST/FOLLOW/LL(1) reference. Usage: python ref_check.py YourGrammar.gram
# Prints conflicts and writes ref_sets.json for diffing against CkLL1's log.
import re, sys
import sys
src = open(sys.argv[1], encoding='latin-1').read().replace('\r','')
# strip // comments
src = re.sub(r'//[^\n]*', '', src)
gi = src.index('GRAMMAR\n')
gram = src[gi+len('GRAMMAR\n'):]
terms_decl = set(re.findall(r'"([^"]+)"', src[:gi]))
toks = re.findall(r"'[^']*'|->|;|\.|[A-Za-z_][A-Za-z0-9_]*", gram)
prods = {}  # LHS -> list of rhs lists
order = []
i = 0
lhs = None
cur = None
while i < len(toks):
    t = toks[i]
    if t == ';':
        lhs = None; cur = None
    elif t == '->':
        if lhs is None:
            raise SystemExit('arrow w/o lhs')
        cur = []
        prods[lhs].append(cur)
    elif lhs is None:
        lhs = t
        if lhs not in prods:
            prods[lhs] = []; order.append(lhs)
    else:
        if t == '.':
            cur.append('EPS')
        elif t.startswith("'"):
            cur.append(t)
        else:
            cur.append(t)
    i += 1
NT = set(prods)
EPS='EPS'
# FIRST
first = {n:set() for n in NT}
def fs(sym):
    if sym == EPS: return {EPS}
    if sym in NT: return first[sym]
    return {sym}
def first_seq(seq):
    out=set()
    for s in seq:
        f=fs(s)
        out |= f-{EPS}
        if EPS not in f: return out
    out.add(EPS); return out
ch=True
while ch:
    ch=False
    for A in NT:
        for r in prods[A]:
            f=first_seq(r)
            if not f<=first[A]:
                first[A]|=f; ch=True
follow={n:set() for n in NT}
start=order[0]; follow[start].add('$')
ch=True
while ch:
    ch=False
    for A in NT:
        for r in prods[A]:
            for i,s in enumerate(r):
                if s in NT:
                    f=first_seq(r[i+1:])
                    add=(f-{EPS})
                    if EPS in f: add|=follow[A]
                    if not add<=follow[s]:
                        follow[s]|=add; ch=True
nullable={n for n in NT if EPS in first[n]}
import json
json.dump({'first':{k:sorted(v) for k,v in first.items()},
           'follow':{k:sorted(v) for k,v in follow.items()},
           'nullable':sorted(nullable),'order':order,
           'prods':prods}, open('ref_sets.json','w'), indent=1)
print('nonterminals',len(NT),'rules',sum(len(v) for v in prods.values()))
print('nullable count',len(nullable))
# undefined / undeclared terminals
used=set(s for A in NT for r in prods[A] for s in r if s!=EPS and s not in NT)
print('terminals used but not declared:', sorted(t for t in used if t.strip("'") not in terms_decl))
# LL(1) conflicts
conf=[]
for A in order:
    rs=prods[A]
    sets=[]
    for r in rs:
        f=first_seq(r); s=set(f-{EPS})
        if EPS in f: s|=follow[A]
        sets.append(s)
    for a in range(len(rs)):
        for b in range(a+1,len(rs)):
            inter=sets[a]&sets[b]
            if inter: conf.append((A,rs[a],rs[b],sorted(inter)))
print('reference LL(1) conflicts (cells):', len(conf))
for c in conf: print(' ',c[0],'|',' '.join(c[1]),'|',' '.join(c[2]),'|',c[3][:6], '...' if len(c[3])>6 else '')
