function arg(a) {
    return a;
}

function sum(a, b) {
    if(isNaN(a) ){
            return("введите числовое значение аргумента а");
    }
    else{
        if(isNaN(b)){
            return("введите числовое значение аргумента b");
        } 
    else return a + b;
    }
}

function mass() {
    let arr = [null, 1, "Два", 3, 4, 5, NaN];
    console.log("Исходные данные");
    console.log(arr);
    let newarr = arr.map((value) =>
        isNaN(value) ? value = 0 : value *= value
        )
    console.log("Полученные данные");
    console.log(newarr);
}
