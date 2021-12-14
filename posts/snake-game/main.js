var app = new Vue({
    el: '#app',
    data() {
        return {
            cell_width: 10,
            height: 400,
            rows: 40,
            columns: 40,
            grid: [],
            snake: [[0,0],[1,0]],
            running: false,
            direction: 'down',
            consumed: true,
            snake_color: 'white',
            food: [],
            direction_map: {
                'up': [-1,0],
                'down': [1,0],
                'right': [0, 1],
                'left': [0,-1]
            },
            movement: {
                'up': {
                    38: 'up',
                    40: 'up',
                    37: 'left',
                    39: 'right'
                },
                'down': {
                    38: 'down',
                    40: 'down',
                    37: 'left',
                    39: 'right'
                },
                'right': {
                    38: 'up',
                    40: 'down',
                    37: 'right',
                    39: 'right'
                },
                'left': {
                    38: 'up',
                    40: 'down',
                    37: 'left',
                    39: 'left'
                }
            }
        }
    },
    computed: {
        box_style() {
            return {
                height: this.height + 'px',
                width: "100%"
            }
        },
        game_state() {
            return this.running ? "Stop": "Play"
        },
        points(){
            return this.snake.length
        },
        snake_color() {
            return this.running ? 'blue' : 'red';
        }
    },
    created(){
        this.init_grid();
        document.addEventListener("keyup", this.change_direction);
        window.addEventListener("resize", this.calculate_columns);
        
    },
    destroyed() {
        window.removeEventListener("resize", this.calculate_columns);
    },
    mounted(){
        this.calculate_columns()
    },
    watch:{
        running(){
            if (this.running)
                this.game_loop()
        },
        food(){
            this.grid[this.food[0]][this.food[1]] = true
        },
        snake(){
            this.snake.forEach(pos => {
                this.grid[pos[0]][pos[1]] = true
            })
        }
    },
    methods: {
        calculate_columns(){
            this.columns = parseInt(this.$refs.box.clientWidth/this.cell_width)
        },
        cell_style(i, j){
            return {
                'background-color': this.grid[i-1][j-1] ? this.snake_color : 'black'
            }
        },
        init_grid(){
            this.grid = [];
            for (var i = 0; i < this.rows; i++){
                Vue.set(this.grid, i, [])
                for (var j = 0; j < this.columns; j++){
                    Vue.set(this.grid[i], j, false)
                }
            }
        },
        game_loop(){
            if(this.running){
                this.iteration += 1
                var killed = this.update_snake_and_food()
                if (!killed) requestAnimationFrame(this.game_loop);
                else this.running = false
            }
            
        },
        update_snake_and_food(){
            this.move_snake()
            if(this.consumed){
                this.add_food()
                this.consumed = false
            }
            return false
        },
        get_random_cell(){
            return [parseInt(Math.random() * this.rows),
            parseInt(Math.random() * this.columns)]
        },
        add_food() {
            this.food = this.get_random_cell();
        },
        is_food_on(x,y){
            return this.food[0] == x && this.food[1] == y
        },
        is_killed(x, y){
            var bite = false
            this.snake.forEach(cell => {
                if(!bite)
                    bite = cell[0] == x && cell[1] == y
            })
            return bite
        },
        move_snake(){
            var factors = this.direction_map[this.direction]
            var head = [...this.snake[this.snake.length - 1]]
            // console.log("factors", factors, this.direction, head)
            head[0] += factors[0]
            if (head[0] >= this.rows) head[0] = 0
            if (head[0] < 0 ) head[0] = this.rows - 1
            head[1] += factors[1]
            if (head[1] >= this.columns) head[1] = 0
            if (head[1] < 0 ) head[1] = this.columns - 1
            if(this.is_killed(head[0], head[1])){
                this.running = false
            }
            this.snake.push(head)
            if (this.is_food_on(head[0], head[1])){
                this.consumed = true
            }else{
                var old_positions = this.snake.splice(0, 1)
                if(old_positions && old_positions.length > 0) {
                    var p = old_positions[0]
                    this.grid[p[0]][p[1]] = false
                }
            }
        },
        change_direction(event){
            // var up = 38
            // var down = 40
            // var left = 37
            // var right = 39
            event.preventDefault()
            this.direction = this.movement[this.direction][event.keyCode]
        },
    }
})
