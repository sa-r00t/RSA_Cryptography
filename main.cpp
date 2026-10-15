#include <iostream>     
#include <random>       
#include <boost/multiprecision/cpp_int.hpp>  
using namespace std;
using namespace boost::multiprecision;

cpp_int big_int(int bits) {
	static random_device rd;
	static mt19937_64 gen(rd());

	cpp_int result;

	int iteration = bits / 64;

	while (iteration != 0) {
		//each chunks is 64 bits 
		cpp_int chunk = gen();
		result = (result << 64) | chunk;
		-- iteration ;
	}

	return result;

}

bool isprime(cpp_int value){
	
	// this is part of the miller-rabin primality test
	//here we test the small cases first

	if (value < 2){
		return false;
	}
	else if (value == 2 or value == 3) {
		return true;
	}
	else if (value % 2 == 0) {
		return false;
	}
	else if (value % 3 == 0) {
		return false;
	}

	//find d and r so we can test with different bases later on 

	cpp_int nminus = value - 1;

	cpp_int d = value - 1;

	int r = 0;

	while (d % 2 == 0) {
		d = d / 2;
		r++;
	}

	//now that we found d and r we loop with each bases

	int bases[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37 };
	
	for (int base : bases) {
		cpp_int x = powm(cpp_int(base), d, value);

		if (x == 1 || x == value - 1) {
			continue;
		}

		bool passed = false;
		for (int i = 0; i < r - 1; i++) {
			x = powm(x, 2, value);
			if (x == value - 1) {
				passed = true;
				break;
			}
		}

		if (!passed) {
			return false;
		}
	}

	return true;


}

cpp_int mod_inverse(cpp_int e, cpp_int phiN) {
	cpp_int r1 = phiN;
	cpp_int r2 = e;
	cpp_int t1 = 0;
	cpp_int t2 = 1;

	while (r2 > 0) {
		cpp_int q = r1 / r2;
		cpp_int r = r1 - (q * r2);
		cpp_int t = t1 - (q * t2);

		r1 = r2;
		r2 = r;
		t1 = t2;
		t2 = t;
	}



	// r1 should be 1 (gcd of e and phiN)
	if (r1 != 1) {
		// Inverse doesn't exist (shouldn't happen for valid RSA keys)
		return 0;
	}

	if (t1 < 0) {
		t1 += phiN;
	}

	return t1;
}


cpp_int mod_pow(cpp_int base, cpp_int exp, cpp_int mod) {
	cpp_int result = 1;
	base %= mod;

	while (exp > 0) {
		if (exp & 1) {
			result = (result * base) % mod;
		}
		base = (base * base) % mod;
		exp >>= 1;
	}

	return result;
}


cpp_int gen_prime(int bits) {
	while (true) {
	
		cpp_int candidate = big_int(bits);
	if (candidate % 2 == 0) {
		candidate++;
	}
	if (isprime(candidate) == true) {
		return candidate;
	}
	
	}
}



//structure created to be able to return the 3 essential values to encrypt and decrypt stuff in rsa

struct RSAKEYS {

	cpp_int n;
	cpp_int e;
	cpp_int d;

};

// we declare a key generation function using our structure so we can return each value of n,e and d

RSAKEYS keygen(int bits) {
	// bits number are multiplied by 2 as we multiply p and q ex: bits = 1024 -> n = 2048 bits
	cpp_int e = 65537;

	cpp_int p = gen_prime(bits), q = gen_prime(bits);

	cpp_int n = p * q;

	cpp_int phiN = (p - 1) * (q - 1);


	while (gcd(e, phiN) != 1) {
		e += 2;
	}

	cpp_int d = mod_inverse(e, phiN);


	RSAKEYS keys;
	keys.n = n;
	keys.e = e;
	keys.d = d;
	

	return keys;
}

cpp_int str_to_m(string message) {
	cpp_int result = 0;

	for (char letter : message) {
		int ascii = (int)letter;
		result = (result << 8) | ascii;
	}

	return result;
}


string m_to_str(cpp_int number) {
	string result = "";


	while (number > 0) {

		int ascii = (int)(number & 0xFF);  
		result = char(ascii) + result;     
		number >>= 8;                       
	}

	return result;
}



cpp_int encrypt(string m,cpp_int e, cpp_int n) {
	cpp_int message = str_to_m(m);
	return mod_pow(message,e,n);

}


string decrypt(cpp_int c, cpp_int d, cpp_int n) {
	
	
	return m_to_str(mod_pow(c, d, n));
	


}

int main() {
	cout << "Generating keys ...\n\n\n";

	RSAKEYS keys = keygen(1024);
	cout << "n = " << keys.n << "\n\n";
	cout << "e = " << keys.e << "\n\n";
	cout << "d = " << keys.d << "\n\n";
	

	
	
	cout << "numerical represntation of the word Hello : " << str_to_m("Hello") << "\n\n";

	cout << "m = Hello (Word To Encrypt) \n\n";
	cout << "Encrypted m (Hello) in RSA: " << encrypt("Hello", keys.e, keys.n) << "\n\n";
	cout << "Decrypted m (Hello) in RSA: " << decrypt(encrypt("Hello", keys.e, keys.n), keys.d, keys.n) << "\n\n";
	return 0;
}

