package main

import (
	"fmt"
	"strconv"
	"os"
)

func main() {
	n ,_ := strconv.Atoi(os.Args[1])
	e:=(n*(n-1)) + n
	fmt.Println("",(n+2),e)
	for i:=1;i<=n/2;i++{
		fmt.Println(" 0",i,10000)
	}
	for i:=1;i<=n;i++{
		s:=i
		d:=i+1
		c:=40
		if i==n{
			d=1
		}
		fmt.Println("",s,d,c)
		fmt.Println("",d,s,c)
		for j:=i+2;j<=n && (j+1)%n!=i ;j++{
			d=j
			fmt.Println("",s,d,c/2)
			fmt.Println("",d,s,c/2)
		}
	}

	for i:=n/2+1;i<=n;i++{
		fmt.Println("",i,(n+1),10000)
	}
}