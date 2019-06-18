#include <stdio.h>
#include <time.h>
#include <set>
#include <utility>
#include <deque>
#include <string>
#include <map>
#include <algorithm>
#include <math.h>

using namespace std;

typedef long long lli;
typedef pair<lli, lli> ip;

map<lli, lli> ts;
lli curts;

lli gett(lli x) {
	if (!ts.count(x)) return -1;
	else return ts[x];
}

class ARC {
public:
	int csz, p;
	double rat;
	set<ip> t1, b1, t2, b2;
	void clear() {
		t1.clear(); b1.clear(); t2.clear(); b2.clear();
	}
	bool chk() {
		return t1.size() <= p &&
			t2.size() <= csz - p &&
			t1.size() + b1.size() <= (int)((2 * csz) * rat) &&
			t1.size() + b1.size() + t2.size() + b2.size() <= 2 * csz;
	}
	void adjust() {
		while (t1.size() > p) {
			b1.insert(*t1.begin());
			t1.erase(t1.begin());
		}
		while (t2.size() > csz - p) {
			b2.insert(*t2.begin());
			t2.erase(t2.begin());
		}
	}
	bool hit(lli x) {
		lli t = gett(x);
		if (t1.count(ip(t, x)) || t2.count(ip(t, x))) return true;
		else return false;
	}
	void process(lli x) {
		lli t = gett(x);
		if (hit(x)) {
			if (t1.count(ip(t, x))) {
				t1.erase(ip(t, x));
				t2.insert(ip(curts, x));
			}
			else {
				t2.erase(ip(t, x));
				t2.insert(ip(curts, x));
			}
		}
		else if (b1.count(ip(t, x))) {
			int mv = (b1.size() >= b2.size() ? 1 : b2.size() / b1.size());
			//int _mv = 2;
			//int _mv = log(1.0*mv) / log(2.0) + 1e-9;
			int _mv = pow(2.0, mv*1.0);
			if (_mv == 0) _mv = 1;
			p = min(p + _mv, (int)((2 * csz) * rat));
			b1.erase(ip(t, x));
			t2.insert(ip(curts, x));
		}
		else if (b2.count(ip(t, x))) {
			int mv = (b1.size() <= b2.size() ? 1 : b1.size() / b2.size());
			//int _mv = 2;
			//int _mv = log(1.0*mv) / log(2.0) + 1e-9;
			int _mv = pow(2.0, mv*1.0);
			if (_mv == 0) _mv = 1;
			p = max(p - _mv, 0);
			b2.erase(ip(t, x));
			t2.insert(ip(curts, x));
		}
		else {
			if (t1.size() + b1.size() == (int)((2 * csz) * rat)) {
				if (t1.size() < (int)((2 * csz) * rat)) {
					b1.erase(b1.begin());
				}
				else {
					t1.erase(t1.begin());
				}
			}
			else {
				if (t1.size() + t2.size() + b1.size() + b2.size() >= csz) {
					if (t1.size() + t2.size() + b1.size() + b2.size() >= csz * 2) {
						if (b2.empty()) t2.erase(t2.begin());
						else b2.erase(b2.begin());
					}
				}
			}
			t1.insert(ip(curts, x));
		}
		adjust();
	}
};

ARC arc;

string ss[8] = { "output_mail.txt","final_exchange.txt","output_home.txt","output_web_vm.txt","final_rbes.txt","final_msnscfs.txt","final_ra.txt","final_dads.txt" };
string tt[8] = { "output_mail_result.txt","final_exchange_result.txt","output_home_result.txt","output_web_vm_result.txt","final_rbes_result.txt","final_msnscfs_result.txt","final_ra_result.txt","final_dads_result.txt" };

int main() {
	for (int i = 0; i < 8; i++) {
		printf("file %d : %s\n", i, ss[i].c_str());
		FILE *in = fopen(ss[i].c_str(), "r");
		FILE* out = fopen(tt[i].c_str(), "w");
		ts.clear();
		arc.clear();
		curts = 0;
		arc.p = 0;
		arc.csz = 8192 * 16;
		arc.rat = 0.5;
		long long clk = clock();
		char dd[1111];
		fgets(dd, 1110, in);
		lli t, o, c;
		long long count = 0;
		long long hit = 0, miss = 0;
		int ll = 0;
		while (fscanf(in, "%lld %lld %lld", &t, &o, &c) == 3) {
			ll++;
			count += c;
			curts = t;
			for (lli x = o; x < o + c; x++) {
				if (arc.hit(x)) hit++;
				else miss++;
			}
			for (lli x = o; x < o + c; x++) {
				arc.process(x);
				ts[x] = t;
			}
			if (ll % 2000000 == 0) {
				printf("processed %d lines\n", ll);
			}
			if (ll == 20000000) break;
		}
		fprintf(out, "lines = %d\n", ll);
		fprintf(out, "total size = %d\n", (int)ts.size());
		fprintf(out, "accessed page = %lld\n", count);
		fprintf(out, "hit = %lld, miss = %lld, hit ratio = %.6f\n", hit, miss, 1.0*hit / (hit + miss));
		fprintf(out, "execution time : %.3f\n", (1.0*clock() - clk) / CLOCKS_PER_SEC);
		fclose(in); fclose(out);
	}
	return 0;
}