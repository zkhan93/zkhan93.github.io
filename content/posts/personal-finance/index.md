---
title: "Personal Finance"
date: 2022-10-31T04:06:04Z
categories: [diy]
tags: ["finance", "fireflyIII", "money", "personal"]

---

One fine day a throught came by that its been 6 Years since I started working, and I have not given enough attention on my financial situation. no financial investments or goals.

Initially I thougt I can live off a excel sheet where I can set up my earnings, expenses and goals and then see how I'm doing.
but things got complicated, when I wanted to increase earning yearly, and based on that I wanted to project the contributions to a goal.

so I looked for a exiting open source system that has all that I need. 
there was a project FireflyIII which is a good self hosted system with a lots of features, after trying to setup everything on it for a couple of hours, I wasn't feeling settled. The web interface feels a little old, and it lacks forcasting which was esential for me.

So I set out to build a small financal system which should be simple to setup and should quickly show what I needed to see.
and would have a modern UI.

I love Vuejs so I started the project in vue3 with quassar framework.
that input to the system is a config JSON which can describe income, budgets, goals, piggy banks etc.

# Income
Income is anything that you earn periodically.

# Budget
Budget is the expected amount that you will spend monthly or yearly, like grocerries, phone, rent etc.
Actual bills can be captured against each budget item to see if you stay within the budget, 
but on a high level this amount is meant to be deducted from your income.

# Goals (or maybe "Piggy banks")
A goal is a fixed amount that you want to collect for a reason,
say you want 10,000 USD to buy a car, thats a goal.
Goals will have priority and start date.
And all your savings(income - expenses) will be distributed among these goals according to the priority.

# Piggy Banks (or maybe "Saving Accounts")
Piggy banks are accounts that you contribute a fixed amount periodically. 
they don't have a target amount.

we can debate on what we should call them but these are the 4 things that will drive the system.
The system work by reading a JSON where all income, budget, goal and piggy banks are defined and creating transactions that represent the expenses the user would do, 
so like if the config is like this

```js
{
    "incomes": [{
        id: "salary",
        name: "Salary",
        amount: 9999,
        currency: "USD",
        recurring:{
            type: "MONTHLY",
            day_of_month: 27
        }
    }],
    budgets:[{
        id: "rent",
        name: "Rent",
        amount: 2000,
        currency: "USD",
         recurring:{
            type: "MONTHLY",
            day_of_month: 5
        }
    }]
}
```
then if you give a date `2023-01-30` till which the system has to generate the transactions, then this would be the result

```js
transactions:[
    {
        id:1,
        amount: 9999,
        type: "credit",
        date: "2022-12-27"
        description: "Salary credit for Dec"
    },
     {
        id:1,
        amount: -2000,
        type: "debit",
        date: "2022-01-05"
        description: "Rent debit for Jan"
    },
]
```

now you see how the system can now produce transactions would be made in future, using this system you can see what will be your account balance on a future date. more or less.
In addition to generated list of transaction system should also accept transactions records from the user.
so there is the concept of actuals vs expected transactions against the budget.

The system is currently in develpment phase.

To be continued....