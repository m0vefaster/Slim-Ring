
package main

import (
    "fmt"
)

func main() {
    n :=15

    e:=2*(n*n*(n-1)) + 106//(n*n)
    fmt.Println("",(n*n+2),e)

    var torusId [16][16]int//n+1 * n+1

    count:=1
    for i:=1;i<=n;i++{
        for j:=1;j<=n;j++{
            torusId[i][j]=count
            count++;
        }
    }

   /* for i:=1;i<=n;i++{
        for j:=1;j<=n/2;j++{
            fmt.Println(" 0",torusId[i][j],10000)
        }
    }*/


    s := []int {0,4,3,4,3,4,3,4,3,4,3,4,3,4,3,4}

    for i:=1;i<=n;i++{
        for j:=1;j<=s[i];j++{
            fmt.Println(" 0",torusId[i][j],10000)
            fmt.Println("",torusId[i][n-j],(n*n+1),10000)
        }
    }

    for i:=1;i<=n;i++{
        for j:=1;j<=n;j++{
            s:=torusId[i][j]
            var d int
            if j==n{
                d=torusId[i][1]
            }else{
                d=torusId[i][j+1]
            }
            c:=40
            fmt.Println("",s,d,c)
            fmt.Println("",d,s,c)
            for k:=j+2;k<=n && (k+1)%n!=j ;k++{
                d=torusId[i][k]
                fmt.Println("",s,d,c/2)
                fmt.Println("",d,s,c/2)
            }
        }
    }


    for i:=1;i<=n;i++{
        for j:=1;j<=n;j++{
            s:=torusId[j][i]
            var d int
            if j==n{
                d=torusId[1][i]
            }else{
                d=torusId[j+1][i]
            }
            c:=40
            fmt.Println("",s,d,c)
            fmt.Println("",d,s,c)
            for k:=j+2;k<=n && (k+1)%n!=j ;k++{
                d=torusId[k][i]
                fmt.Println("",s,d,c/2)
                fmt.Println("",d,s,c/2)
            }
        }
    }

/*for i:=1;i<=n;i++{
        for j:=n/2+1;j<=n;j++{
            fmt.Println("",torusId[i][j],(n*n+1),10000)
        }
    }*/
}